using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using AdrGaspard.ChokefishSuite.Core.Utils;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.ComponentModel;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class MatchSchedulerViewModel : ObservableObject
    {
        private readonly object _lock;
        private bool _initializing;
        private bool _running;
        private bool _errorEncountered;
        private ulong _thinkTimeInMs;
        private string? _whiteEngineName;
        private string? _blackEngineName;
        private RatioViewModel _ratioVM;
        private MatchMakerViewModel? _matchMakerVM;
        private EngineSelectorViewModel _firstEngineSelectorVM;
        private EngineSelectorViewModel _secondEngineSelectorVM;
        private CancellationTokenSource _cancellationTokenSource;
        private TaskCompletionSource<bool> _matchCompletionSource;

        public MatchSchedulerViewModel()
        {
            _lock = new();
            _initializing = false;
            _running = false;
            _errorEncountered = false;
            _thinkTimeInMs = 200;
            _ratioVM = new();
            _matchMakerVM = null;
            _firstEngineSelectorVM = new();
            _secondEngineSelectorVM = new();
            _cancellationTokenSource = new();
            _matchCompletionSource = new();
            StartMatchScheduleCommand = new RelayCommand(StartMatchSchedule);
            StopMatchScheduleCommand = new RelayCommand(StopMatchSchedule);
            ResetCommand = new RelayCommand(Reset);
        }

        public virtual MatchMakerViewModel? MatchMakerVM
        {
            get => _matchMakerVM;
            protected set
            {
                if (_matchMakerVM != value)
                {
                    if (_matchMakerVM is not null)
                    {
                        UnsubsbribeFromMatchMakerPropertyChanged(_matchMakerVM);
                    }
                    _ = SetProperty(ref _matchMakerVM, value);
                    if (_matchMakerVM is not null)
                    {
                        SubsbribeToMatchMakerPropertyChanged(_matchMakerVM);
                    }
                }
            }
        }

        public RatioViewModel RatioVM
        {
            get => _ratioVM;
            private set => SetProperty(ref _ratioVM, value);
        }

        public EngineSelectorViewModel FirstEngineSelectorVM
        {
            get => _firstEngineSelectorVM;
            private set => SetProperty(ref _firstEngineSelectorVM, value);
        }

        public EngineSelectorViewModel SecondEngineSelectorVM
        {
            get => _secondEngineSelectorVM;
            private set => SetProperty(ref _secondEngineSelectorVM, value);
        }

        public bool Initializing
        {
            get => _initializing;
            private set => SetProperty(ref _initializing, value);
        }

        public bool Running
        {
            get => _running;
            private set => SetProperty(ref _running, value);
        }

        public bool ErrorEncountered
        {
            get => _errorEncountered;
            private set => SetProperty(ref _errorEncountered, value);
        }

        public ulong ThinkTimeInMs
        {
            get => _thinkTimeInMs;
            set
            {
                if (!Running && value > 0)
                {
                    SetProperty(ref _thinkTimeInMs, value);
                }
            }
        }

        public string? WhiteEngineName
        {
            get => _whiteEngineName;
            private set => SetProperty(ref _whiteEngineName, value);
        }

        public string? BlackEngineName
        {
            get => _blackEngineName;
            private set => SetProperty(ref _blackEngineName, value);
        }

        public ICommand StartMatchScheduleCommand { get; private init; }

        public ICommand StopMatchScheduleCommand { get; private init; }

        public ICommand ResetCommand { get; private init; }

        private void StartMatchSchedule()
        {
            _ = Task.Run(() =>
            {
                lock (_lock)
                {
                    if (!Running)
                    {
                        Initializing = true;
                        if (!FirstEngineSelectorVM.IsEnginePathValid || !SecondEngineSelectorVM.IsEnginePathValid)
                        {
                            Initializing = false;
                            ErrorEncountered = true;
                            return;
                        }
                        if (!_cancellationTokenSource.IsCancellationRequested)
                        {
                            _cancellationTokenSource.Cancel();
                        }
                        _cancellationTokenSource = new();
                        CancellationToken token = _cancellationTokenSource.Token;
                        RatioVM.ResetCommand.Execute(null);
                        TaskCompletionSource<bool> _startupCompletionSource = new();
                        _ = Task.Run(() =>
                        {
                            int i = 0;
                            try
                            {
                                IChessEngine firstEngine = CreateAndInitializeEngine(FirstEngineSelectorVM);
                                IChessEngine secondEngine = CreateAndInitializeEngine(SecondEngineSelectorVM);
                                WhiteEngineName = FirstEngineSelectorVM.EngineName;
                                BlackEngineName = SecondEngineSelectorVM.EngineName;
                                Running = true;
                                Initializing = false;
                                _startupCompletionSource.SetResult(true);
                                while (!token.IsCancellationRequested)
                                {
                                    MatchMakerVM?.StopMatchCommand.Execute(null);
                                    MatchMakerVM = new(firstEngine, secondEngine, UciCommands.PositionArgumentStartpos, ThinkTimeInMs);
                                    MatchMakerVM.MatchCompleted += OnMatchCompletedOrCancelled;
                                    MatchMakerVM.MatchCanceled += OnMatchCompletedOrCancelled;
                                    _matchCompletionSource = new();
                                    MatchMakerVM.StartMatchCommand.Execute(null);
                                    _matchCompletionSource.Task.Wait();
                                    UpdateRatio(MatchMakerVM.Result);
                                    MatchMakerVM.MatchCanceled -= OnMatchCompletedOrCancelled;
                                    MatchMakerVM.MatchCompleted -= OnMatchCompletedOrCancelled;
                                    if (i++ > 1000)
                                    {
                                        break;
                                    }
                                }
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

        private void StopMatchSchedule()
        {
            lock (_lock)
            {
                if (Running)
                {
                    Running = false;
                    MatchMakerVM!.StopMatchCommand.Execute(null);
                    MatchMakerVM!.MatchCanceled -= OnMatchCompletedOrCancelled;
                    MatchMakerVM!.MatchCompleted -= OnMatchCompletedOrCancelled;
                    _cancellationTokenSource.Cancel();
                }
            }
        }

        private void Reset()
        {
            if (!Running)
            {
                MatchMakerVM = null;
                RatioVM.ResetCommand.Execute(null);
                ErrorEncountered = false;
                WhiteEngineName = null;
                BlackEngineName = null;
            }
        }

        private IChessEngine CreateAndInitializeEngine(EngineSelectorViewModel engineSelectorVM)
        {
            IChessEngine engine = new UciChessEngine(engineSelectorVM.UseWsl
                ? new IOTransmitter("wsl", engineSelectorVM.EnginePath.ToWslPath(), "\n")
                : new IOTransmitter(engineSelectorVM.EnginePath));
            engine.Initialize();
            engine.SetOption(OptionHelper.PonderOptionName, false);
            return engine;
        }

        private void UpdateRatio(ChessGameResult result)
        {
            switch (result)
            {
                case ChessGameResult.WhiteMated:
                    RatioVM.DefeatCount++;
                    break;
                case ChessGameResult.BlackMated:
                    RatioVM.VictoryCount++;
                    break;
                case ChessGameResult.Stalemate:
                case ChessGameResult.Repetition:
                case ChessGameResult.FiftyMoveRule:
                case ChessGameResult.InsufficientMaterial:
                    RatioVM.DrawCount++;
                    break;
                case ChessGameResult.Playing:
                case ChessGameResult.None:
                default:
                    break;
            }
        }

        private void SubsbribeToMatchMakerPropertyChanged(MatchMakerViewModel matchMaker)
        {
            matchMaker.PropertyChanged += OnMatchMakerPropertyChanged;
        }

        private void UnsubsbribeFromMatchMakerPropertyChanged(MatchMakerViewModel matchMaker)
        {
            matchMaker.PropertyChanged -= OnMatchMakerPropertyChanged;
        }

        private void OnMatchCompletedOrCancelled(object? sender, EventArgs eventArgs)
        {
            _matchCompletionSource.SetResult(true);
        }

        protected virtual void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
        }
    }
}
