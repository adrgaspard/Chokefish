using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using AdrGaspard.ChokefishSuite.Core.Utils;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public abstract class MatchViewModelBase : ObservableObject
    {
        protected readonly object _lock;
        private bool _initializing;
        private bool _running;
        private bool _errorEncountered;
        private CancellationTokenSource _cancellationTokenSource;
        private TaskCompletionSource<bool> _startupCompletionSource;

        protected MatchViewModelBase()
        {
            _lock = new();
            _initializing = false;
            _running = false;
            _errorEncountered = false;
            _cancellationTokenSource = new();
            _startupCompletionSource = new();
            StartMatchCommand = new RelayCommand(StartMatch);
            StopMatchCommand = new RelayCommand(StopMatch);
            ResetCommand = new RelayCommand(Reset);
        }

        public bool Initializing
        {
            get => _initializing;
            protected set => SetProperty(ref _initializing, value);
        }

        public bool Running
        {
            get => _running;
            protected set
            {
                if (_running != value)
                {
                    SetProperty(ref _running, value);
                    OnPropertyChanged(nameof(MatchCanStart));
                }
            }
        }

        public bool ErrorEncountered
        {
            get => _errorEncountered;
            protected set
            {
                if (_errorEncountered != value)
                {
                    SetProperty(ref _errorEncountered, value);
                    OnPropertyChanged(nameof(MatchCanStart));
                }
            }
        }

        public virtual bool MatchCanStart => !Running && !ErrorEncountered;

        public ICommand StartMatchCommand { get; private init; }

        public ICommand StopMatchCommand { get; private init; }

        public ICommand ResetCommand { get; private init; }

        protected virtual void StartMatch()
        {
            Task.Run(() =>
            {
                lock (_lock)
                {
                    if (!Running)
                    {
                        Initializing = true;
                        Reset();
                        if (!_cancellationTokenSource.IsCancellationRequested)
                        {
                            _cancellationTokenSource.Cancel();
                        }
                        _cancellationTokenSource = new();
                        CancellationToken token = _cancellationTokenSource.Token;
                        _startupCompletionSource = new();
                        if (!MatchCanStart)
                        {
                            Initializing = false;
                            ErrorEncountered = true;
                            return;
                        }
                        Task.Run(() =>
                        {
                            try
                            {
                                StartMatchProcedure(token);
                            }
                            catch
                            {
                                Initializing = false;
                                ErrorEncountered = true;
                            }
                            lock (_lock)
                            {
                                if (!token.IsCancellationRequested)
                                {
                                    Running = false;
                                }
                            }
                        });
                        _startupCompletionSource.Task.Wait();
                    }
                }
            });
        }

        private void StopMatch()
        {
            lock (_lock)
            {
                if (Running)
                {
                    Running = false;
                    OnMatchStopped();
                    _cancellationTokenSource.Cancel();
                }
            }
        }

        private void Reset()
        {
            if (!Running)
            {
                ErrorEncountered = false;
                OnReset();
            }
        }

        protected void NotifyInitializationFinished()
        {
            Running = true;
            Initializing = false;
            _startupCompletionSource.TrySetResult(true);
        }

        protected abstract void StartMatchProcedure(CancellationToken token);

        protected abstract void OnMatchStopped();

        protected abstract void OnReset();

        protected static IChessEngine CreateAndInitializeEngine(EngineSelectorViewModel engineSelectorVM)
        {
            IChessEngine engine = new UciChessEngine(engineSelectorVM.UseWsl
                ? new IOTransmitter("wsl", engineSelectorVM.EnginePath.ToWslPath(), "\n")
                : new IOTransmitter(engineSelectorVM.EnginePath));
            engine.Initialize();
            engine.SetOption(OptionHelper.PonderOptionName, false);
            return engine;
        }
    }
}
