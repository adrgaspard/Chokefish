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
    public class MatchSchedulerViewModel : MatchViewModelBase
    {
        private string? _whiteEngineName;
        private string? _blackEngineName;
        private uint _currentGameId;
        private Position? _currentGamePosition;
        private HypothesisResult _result;
        private TaskCompletionSource<bool> _matchCompletionSource;
        private MatchMakerViewModel? _matchMakerVM;

        public MatchSchedulerViewModel() : base()
        {
            _currentGameId = 0;
            _currentGamePosition = null;
            _result = HypothesisResult.None;
            _matchCompletionSource = new();
            _matchMakerVM = null;
            SchedulingRulesVM = new();
            RatioVM = new();
            FirstEngineSelectorVM = new();
            SecondEngineSelectorVM = new();
            SchedulingRulesVM.PropertyChanged += OnValidatablePropertyChanged;
            FirstEngineSelectorVM.PropertyChanged += OnValidatablePropertyChanged;
            SecondEngineSelectorVM.PropertyChanged += OnValidatablePropertyChanged;
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

        public override bool MatchCanStart => base.MatchCanStart && FirstEngineSelectorVM.IsValid && SecondEngineSelectorVM.IsValid && SchedulingRulesVM.IsValid;

        protected override void StartMatchProcedure(CancellationToken token)
        {
            IReadOnlyList<Position> positions = SchedulingRulesVM.Positions;
            bool firstIsWhite = true;
            using IChessEngine firstEngine = CreateAndInitializeEngine(FirstEngineSelectorVM);
            using IChessEngine secondEngine = CreateAndInitializeEngine(SecondEngineSelectorVM);
            NotifyInitializationFinished();
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
                _matchCompletionSource.Task.Wait(token);
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

        protected override void OnMatchStopped()
        {
            if (MatchMakerVM is not null)
            {
                MatchMakerVM.StopMatchCommand.Execute(null);
                MatchMakerVM.MatchCanceled -= OnMatchCompletedOrCancelled;
                MatchMakerVM.MatchCompleted -= OnMatchCompletedOrCancelled;
            }
        }

        protected override void OnReset()
        {
            MatchMakerVM = null;
            RatioVM.ResetCommand.Execute(null);
            WhiteEngineName = null;
            BlackEngineName = null;
            CurrentGameId = 0;
            CurrentGamePosition = null;
            Result = HypothesisResult.None;
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

        private void OnValidatablePropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(IValidatable.IsValid))
            {
                OnPropertyChanged(nameof(MatchCanStart));
            }
        }
    }
}
