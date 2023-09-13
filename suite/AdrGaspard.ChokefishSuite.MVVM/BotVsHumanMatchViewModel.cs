using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.UCI;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class BotVsHumanMatchViewModel : MatchViewModelBase
    {
        private ChessBoard? _board;
        private ChessMove? _lastMove;
        private SearchDebugInfos? _searchInfos;
        private bool _userCanPlay;
        private TaskCompletionSource<ChessMove> _moveCompletionSource;

        public BotVsHumanMatchViewModel()
        {
            _board = null;
            _lastMove = null;
            _searchInfos = null;
            _userCanPlay = false;
            _moveCompletionSource = new();
            EngineSelectorVM = new();
            ExecutePlayerMoveCommand = new RelayCommand<ChessMove>(ExecutePlayerMove);
            EngineSelectorVM.PropertyChanged += OnValidatablePropertyChanged;
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


        public SearchDebugInfos? SearchInfos
        {
            get => _searchInfos;
            private set => SetProperty(ref _searchInfos, value);
        }

        public bool UserCanPlay
        {
            get => _userCanPlay;
            private set => SetProperty(ref _userCanPlay, value);
        }

        public override bool MatchCanStart => base.MatchCanStart && EngineSelectorVM.IsValid;

        public EngineSelectorViewModel EngineSelectorVM { get; private init; }

        public ICommand ExecutePlayerMoveCommand { get; private init; }

        protected override void StartMatchProcedure(CancellationToken token)
        {
            List<string> moves = new();
            using IChessEngine engine = CreateAndInitializeEngine(EngineSelectorVM);
            engine.Disposed += OnEngineDisposed;
            engine.BoardChanged += OnEngineBoardChanged;
            engine.SearchStopped += OnEngineSearchStopped;
            engine.SearchDebugInfosChanged += OnEngineSearchDebugInfoChanged;
            try
            {
                engine.ResetGame();
                engine.SetPosition(UciCommands.PositionArgumentStartpos, moves);
                UserCanPlay = true;
                NotifyInitializationFinished();
                while (!token.IsCancellationRequested)
                {
                    _moveCompletionSource = new();
                    if (!UserCanPlay)
                    {
                        engine.StartSearch(ChessTimeSystem.CreateDefined(TimeSpan.FromSeconds(1)));
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
            SearchInfos = null;
            UserCanPlay = false;
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
                SearchInfos = engine.SearchDebugInfos;
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
