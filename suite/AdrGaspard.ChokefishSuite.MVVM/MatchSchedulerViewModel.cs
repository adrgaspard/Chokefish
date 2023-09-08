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
        private string? _whiteEngineName;
        private string? _blackEngineName;
        private MatchMakerViewModel? _matchMakerVM;
        private CancellationTokenSource _cancellationTokenSource;
        private TaskCompletionSource<bool> _matchCompletionSource;

        public MatchSchedulerViewModel()
        {
            _lock = new();
            _initializing = false;
            _running = false;
            _errorEncountered = false;
            _matchMakerVM = null;
            _cancellationTokenSource = new();
            _matchCompletionSource = new();
            SchedulingRulesVM = new();
            RatioVM = new();
            FirstEngineSelectorVM = new();
            SecondEngineSelectorVM = new();
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
                    SetProperty(ref _matchMakerVM, value);
                    if (_matchMakerVM is not null)
                    {
                        SubsbribeToMatchMakerPropertyChanged(_matchMakerVM);
                    }
                }
            }
        }

        public SchedulingRulesViewModel SchedulingRulesVM { get; private init; }

        public RatioViewModel RatioVM { get; private init; }

        public EngineSelectorViewModel FirstEngineSelectorVM { get; private init; }

        public EngineSelectorViewModel SecondEngineSelectorVM { get; private init; }

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
            Task.Run(() =>
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
                        Task.Run(() =>
                        {
                            int i = 0;
                            try
                            {
                                using IChessEngine firstEngine = CreateAndInitializeEngine(FirstEngineSelectorVM);
                                using IChessEngine secondEngine = CreateAndInitializeEngine(SecondEngineSelectorVM);
                                WhiteEngineName = FirstEngineSelectorVM.EngineName;
                                BlackEngineName = SecondEngineSelectorVM.EngineName;
                                Running = true;
                                Initializing = false;
                                _startupCompletionSource.SetResult(true);
                                while (!token.IsCancellationRequested)
                                {
                                    MatchMakerVM?.StopMatchCommand.Execute(null);
                                    MatchMakerVM = new(firstEngine, secondEngine, UciCommands.PositionArgumentStartpos, SchedulingRulesVM.ThinkTimeInMs);
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

        private static IChessEngine CreateAndInitializeEngine(EngineSelectorViewModel engineSelectorVM)
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
