using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using AdrGaspard.ChokefishSuite.Core.Utils;
using AdrGaspard.ChokefishSuite.MVVM.Models;
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
        private uint _currentGameId;
        private Position? _currentGamePosition;
        private HypothesisResult _result;
        private MatchMakerViewModel? _matchMakerVM;
        private CancellationTokenSource _cancellationTokenSource;
        private TaskCompletionSource<bool> _matchCompletionSource;

        public MatchSchedulerViewModel()
        {
            _lock = new();
            _initializing = false;
            _running = false;
            _errorEncountered = false;
            _currentGameId = 0;
            _currentGamePosition = null;
            _result = HypothesisResult.None;
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

        public uint CurrentGameId
        {
            get => _currentGameId;
            private set => SetProperty(ref _currentGameId, value);
        }

        public Position? CurrentGamePosition
        {
            get => _currentGamePosition;
            private set => SetProperty(ref _currentGamePosition, value);
        }

        public HypothesisResult Result
        {
            get => _result;
            private set => SetProperty(ref _result, value);
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
                        Reset();
                        IReadOnlyList<Position> positions = SchedulingRulesVM.Positions;
                        if (!FirstEngineSelectorVM.IsEnginePathValid || !SecondEngineSelectorVM.IsEnginePathValid || SchedulingRulesVM.Positions.Count == 0)
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
                        TaskCompletionSource<bool> _startupCompletionSource = new();
                        Task.Run(() =>
                        {
                            try
                            {
                                bool firstIsWhite = true;
                                using IChessEngine firstEngine = CreateAndInitializeEngine(FirstEngineSelectorVM);
                                using IChessEngine secondEngine = CreateAndInitializeEngine(SecondEngineSelectorVM);
                                Running = true;
                                Initializing = false;
                                _startupCompletionSource.TrySetResult(true);
                                while (!token.IsCancellationRequested)
                                {
                                    MatchMakerVM?.StopMatchCommand.Execute(null);
                                    if (firstIsWhite)
                                    {
                                        CurrentGamePosition = positions[(SchedulingRulesVM.RandomizeStartPositionsOrder ? Random.Shared.Next() : (int)CurrentGameId / 2) % positions.Count];
                                    }
                                    MatchMakerVM = new(
                                        firstIsWhite ? firstEngine : secondEngine,
                                        firstIsWhite ? secondEngine : firstEngine,
                                        CurrentGamePosition!.Value,
                                        SchedulingRulesVM.ThinkTimeInMs);
                                    CurrentGameId++;
                                    MatchMakerVM.MatchCompleted += OnMatchCompletedOrCancelled;
                                    MatchMakerVM.MatchCanceled += OnMatchCompletedOrCancelled;
                                    _matchCompletionSource = new();
                                    WhiteEngineName = (firstIsWhite ? FirstEngineSelectorVM : SecondEngineSelectorVM).EngineName;
                                    BlackEngineName = (firstIsWhite ? SecondEngineSelectorVM : FirstEngineSelectorVM).EngineName;
                                    MatchMakerVM.StartMatchCommand.Execute(null);
                                    _matchCompletionSource.Task.Wait();
                                    UpdateRatio(MatchMakerVM.Result);
                                    MatchMakerVM.MatchCanceled -= OnMatchCompletedOrCancelled;
                                    MatchMakerVM.MatchCompleted -= OnMatchCompletedOrCancelled;
                                    Result = Statistics.SequentialProbabilityRatioTest((uint)RatioVM.VictoryCount, (uint)RatioVM.DrawCount, (uint)RatioVM.DefeatCount, 0,
                                        SchedulingRulesVM.EloDifference, SchedulingRulesVM.FalsePositiveRisk, SchedulingRulesVM.FalseNegativeRisk);
                                    if (((Result == HypothesisResult.H0 || Result == HypothesisResult.H1) && CurrentGameId >= SchedulingRulesVM.MinimumGamesCount && !SchedulingRulesVM.KeepGoingWhenConclusive) || CurrentGameId >= SchedulingRulesVM.MaximumGamesCount)
                                    {
                                        break;
                                    }
                                    firstIsWhite = !firstIsWhite;
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
                    if (MatchMakerVM is not null)
                    {
                        MatchMakerVM.StopMatchCommand.Execute(null);
                        MatchMakerVM.MatchCanceled -= OnMatchCompletedOrCancelled;
                        MatchMakerVM.MatchCompleted -= OnMatchCompletedOrCancelled;
                    }
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
                CurrentGameId = 0;
                CurrentGamePosition = null;
                Result = HypothesisResult.None;
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
            _matchCompletionSource.TrySetResult(true);
        }

        protected virtual void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
        }
    }
}
