﻿using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class BotVsHumanMatchViewModel : MatchViewModelBase
    {
        protected const string HumanPlayerName = "Player";

        private ChessBoard? _board;
        private ChessMove? _lastMove;
        private bool _userCanPlay;
        private string? _whiteName;
        private string? _blackName;
        private TaskCompletionSource<ChessMove> _moveCompletionSource;

        public BotVsHumanMatchViewModel()
        {
            _board = null;
            _lastMove = null;
            _userCanPlay = false;
            _moveCompletionSource = new();
            EngineSelectorVM = new();
            EngineParametersVM = new();
            SearchInfosVM = new();
            ExecutePlayerMoveCommand = new RelayCommand<ChessMove>(ExecutePlayerMove);
            EngineSelectorVM.PropertyChanged += OnValidatablePropertyChanged;
            EngineParametersVM.PropertyChanged += OnValidatablePropertyChanged;
        }

        public ChessBoard? Board
        {
            get => _board;
            private set => SetProperty(ref _board, value);
        }

        public ChessMove? LastMove
        {
            get => _lastMove;
            private set => SetProperty(ref _lastMove, value);
        }

        public bool UserCanPlay
        {
            get => _userCanPlay;
            private set => SetProperty(ref _userCanPlay, value);
        }

        public string? WhiteName
        {
            get => _whiteName;
            private set => SetProperty(ref _whiteName, value);
        }

        public string? BlackName
        {
            get => _blackName;
            private set => SetProperty(ref _blackName, value);
        }

        public override bool MatchCanStart => base.MatchCanStart && EngineSelectorVM.IsValid && EngineParametersVM.IsValid;

        public EngineSelectorViewModel EngineSelectorVM { get; private init; }

        public EngineParametersViewModel EngineParametersVM { get; private init; }

        public SearchInfosViewModel SearchInfosVM { get; private init; }

        public ICommand ExecutePlayerMoveCommand { get; private init; }

        protected override void StartMatchProcedure(CancellationToken token)
        {
            List<string> moves = new();
            using IChessEngine engine = CreateAndInitializeEngine(EngineSelectorVM);
            engine.Disposed += OnEngineDisposed;
            engine.BoardChanged += OnEngineBoardChanged;
            engine.SearchStopped += OnEngineSearchStopped;
            engine.SearchDebugInfosChanged += OnEngineSearchDebugInfoChanged;
            ChessTimeSystem timeSystem = ChessTimeSystem.CreateDefined(TimeSpan.FromMilliseconds(EngineParametersVM.ThinkTimeInMs));
            bool isOk = true;
            try
            {
                isOk &= engine.SetDebug(EngineParametersVM.Debug);
                isOk &= engine.SetOption(OptionHelper.PonderOptionName, EngineParametersVM.Ponder);
                isOk &= engine.SetOption(OptionHelper.OwnBookOptionName, EngineParametersVM.OwnBook);
                isOk &= engine.SetOption(OptionHelper.ThreadsOptionName, EngineParametersVM.Threads);
                isOk &= engine.ResetGame();
                isOk &= engine.SetPosition(EngineParametersVM.Position, moves);
                if (!isOk)
                {
                    throw new InvalidOperationException("Something went wrong when initializing the engine!");
                }
                UserCanPlay = EngineParametersVM.HumanPlaysWhite;
                WhiteName = UserCanPlay ? HumanPlayerName : EngineSelectorVM.EngineName;
                BlackName = UserCanPlay ? EngineSelectorVM.EngineName : HumanPlayerName;
                NotifyInitializationFinished();
                while (!token.IsCancellationRequested)
                {
                    _moveCompletionSource = new();
                    if (!UserCanPlay)
                    {
                        engine.StartSearch(timeSystem);
                    }
                    _moveCompletionSource.Task.Wait(token);
                    ChessMove move = _moveCompletionSource.Task.Result;
                    moves.Add(move.ToUciString() ?? throw new NullReferenceException($"An engine search didn't returned a correct move!"));
                    LastMove = move;
                    engine.SetPosition(UciCommands.PositionArgumentStartpos, moves);
                    ChessGameResult result = Board!.Value.Result;
                    if (result != ChessGameResult.Playing)
                    {
                        break;
                    }
                    UserCanPlay = !UserCanPlay;
                }
            }
            catch
            {
                throw;
            }
            finally
            {
                engine.SearchDebugInfosChanged -= OnEngineSearchDebugInfoChanged;
                engine.SearchStopped -= OnEngineSearchStopped;
                engine.BoardChanged -= OnEngineBoardChanged;
                engine.Disposed -= OnEngineDisposed;
            }
        }

        protected override void OnMatchStopped()
        {
            UserCanPlay = false;
        }

        protected override void OnReset()
        {
            Board = null;
            LastMove = null;
            UserCanPlay = false;
            WhiteName = null;
            BlackName = null;
            SearchInfosVM.SetSearchInfosCommand.Execute(null);
        }

        private void ExecutePlayerMove(ChessMove move)
        {
            if (UserCanPlay)
            {
                ExecuteMove(move);
                return;
            }
            throw new InvalidOperationException("The player tried to make a move when it was not his turn.");
        }

        private void ExecuteMove(ChessMove move)
        {
            if (Board is ChessBoard board && board.NextMoves.Contains(move))
            {
                _moveCompletionSource.TrySetResult(move);
                OnMoveExecuted(move);
                return;
            }
            throw new InvalidOperationException("An illegal move was made.");
        }

        private void OnEngineBoardChanged(object? sender, EventArgs eventArgs)
        {
            if (sender is IChessEngine engine)
            {
                Board = engine.Board;
            }
        }

        private void OnEngineSearchDebugInfoChanged(object? sender, EventArgs eventArgs)
        {
            if (sender is IChessEngine engine)
            {
                SearchInfosVM.SetSearchInfosCommand.Execute(engine.SearchDebugInfos);
                return;
            }
            throw new InvalidOperationException($"The engine currently searching is not the right one!");
        }

        private void OnEngineSearchStopped(object? sender, EventArgs eventArgs)
        {
            if (sender is IChessEngine engine && engine.SearchResult?.BestMove is ChessMove bestMove)
            {
                ExecuteMove(bestMove);
            }
        }

        private void OnEngineDisposed(object? sender, EventArgs eventArgs)
        {
            throw new ObjectDisposedException($"The chess engine used was disposed.");
        }
    }
}
