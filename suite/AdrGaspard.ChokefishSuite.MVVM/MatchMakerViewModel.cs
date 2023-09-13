using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class MatchMakerViewModel : ObservableObject
    {
        private readonly object _lock;
        private readonly IChessEngine _whiteEngine;
        private readonly IChessEngine _blackEngine;
        private readonly string _fen;
        private readonly ChessTimeSystem _timeSystem;
        private ChessBoard? _board;
        private ChessMove? _lastMove;
        private ChessGameResult _result;
        private SearchDebugInfos? _whiteSearchDebugInfos;
        private SearchDebugInfos? _blackSearchDebugInfos;
        private ChessColor _searchingColor;
        private bool _running;
        private CancellationTokenSource _cancellationTokenSource;
        private TaskCompletionSource<bool> _searchCompletionSource;

        public MatchMakerViewModel(IChessEngine whiteEngine, IChessEngine blackEngine, string fen, ulong searchTimeInMs)
        {
            _lock = new();
            _whiteEngine = whiteEngine;
            _blackEngine = blackEngine;
            _fen = fen;
            _timeSystem = ChessTimeSystem.CreateDefined(TimeSpan.FromMilliseconds(searchTimeInMs));
            _result = ChessGameResult.None;
            _whiteSearchDebugInfos = null;
            _blackSearchDebugInfos = null;
            _searchingColor = ChessColor.None;
            _running = false;
            _cancellationTokenSource = new();
            _searchCompletionSource = new();
            StartMatchCommand = new RelayCommand(StartMatch);
            StopMatchCommand = new RelayCommand(StopMatch);
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

        public ChessGameResult Result
        {
            get => _result;
            private set => SetProperty(ref _result, value);
        }

        public SearchDebugInfos? WhiteSearchDebugInfos
        {
            get => _whiteSearchDebugInfos;
            private set => SetProperty(ref _whiteSearchDebugInfos, value);
        }

        public SearchDebugInfos? BlackSearchDebugInfos
        {
            get => _whiteSearchDebugInfos;
            private set => SetProperty(ref _blackSearchDebugInfos, value);
        }

        public ICommand StartMatchCommand { get; private init; }

        public ICommand StopMatchCommand { get; private init; }

        public event EventHandler<ChessMove>? MoveExecuted;

        public event EventHandler? MatchCompleted;

        public event EventHandler? MatchCanceled;

        private void StartMatch()
        {
            bool startMatch = false;
            CancellationToken token = CancellationToken.None;
            lock (_lock)
            {
                if (!_running)
                {
                    _running = true;
                    if (!_cancellationTokenSource.IsCancellationRequested)
                    {
                        _cancellationTokenSource.Cancel();
                    }
                    _cancellationTokenSource = new();
                    token = _cancellationTokenSource.Token;
                    startMatch = true;
                }
            }
            if (startMatch)
            {
                Task.Run(() =>
                {
                    lock (_lock)
                    {
                        SubscribeToAllEngineEvents();
                    }
                    _whiteEngine.ResetGame();
                    _blackEngine.ResetGame();
                    _whiteEngine.SetPosition(_fen, Enumerable.Empty<string>());
                    _blackEngine.SetPosition(_fen, Enumerable.Empty<string>());
                    _searchingColor = _whiteEngine.Board?.ColorToMove ?? ChessColor.None;
                    Result = _whiteEngine.Board?.Result ?? ChessGameResult.None;
                    List<string> moves = new();
                    IChessEngine currentEngine = GetCurrentEngine(false);
                    IChessEngine opponentEngine = GetCurrentEngine(true);
                    while (!token.IsCancellationRequested)
                    {
                        _searchCompletionSource = new();
                        currentEngine.StartSearch(_timeSystem);
                        _searchCompletionSource.Task.Wait();
                        ChessMove bestMove = currentEngine.SearchResult?.BestMove ?? throw new NullReferenceException($"An engine search didn't returned a move!");
                        moves.Add(bestMove.ToUciString() ?? throw new NullReferenceException($"An engine search didn't returned a correct move!"));
                        LastMove = bestMove;
                        opponentEngine.SetPosition(_fen, moves);
                        MoveExecuted?.Invoke(this, bestMove);
                        ChessGameResult result = opponentEngine.Board?.Result ?? ChessGameResult.None;
                        if (result != ChessGameResult.Playing)
                        {
                            Result = result;
                            break;
                        }
                        _searchingColor = _searchingColor == ChessColor.White ? ChessColor.Black : ChessColor.White;
                        currentEngine = GetCurrentEngine(false);
                        opponentEngine = GetCurrentEngine(true);

                    }
                    lock (_lock)
                    {
                        if (!token.IsCancellationRequested)
                        {
                            UnsubscribeFromAllEngineEvents();
                            _running = false;
                            MatchCompleted?.Invoke(this, EventArgs.Empty);
                        }
                    }
                });
            }
        }

        private void StopMatch()
        {
            lock (_lock)
            {
                if (_running)
                {
                    _running = false;
                    UnsubscribeFromAllEngineEvents();
                    _cancellationTokenSource.Cancel();
                    Result = ChessGameResult.None;
                    MatchCanceled?.Invoke(this, EventArgs.Empty);
                }
            }
        }

        private void SubscribeToAllEngineEvents()
        {
            _whiteEngine.Disposed += OnEngineDisposed;
            _blackEngine.Disposed += OnEngineDisposed;
            _whiteEngine.BoardChanged += OnBoardChanged;
            _blackEngine.BoardChanged += OnBoardChanged;
            _whiteEngine.SearchStarted += OnSearchStarted;
            _blackEngine.SearchStarted += OnSearchStarted;
            _whiteEngine.SearchDebugInfosChanged += OnSearchDebugInfoChanged;
            _blackEngine.SearchDebugInfosChanged += OnSearchDebugInfoChanged;
            _whiteEngine.SearchStopped += OnSearchStopped;
            _blackEngine.SearchStopped += OnSearchStopped;
        }

        private void UnsubscribeFromAllEngineEvents()
        {
            _whiteEngine.Disposed -= OnEngineDisposed;
            _blackEngine.Disposed -= OnEngineDisposed;
            _whiteEngine.BoardChanged -= OnBoardChanged;
            _blackEngine.BoardChanged -= OnBoardChanged;
            _whiteEngine.SearchStarted -= OnSearchStarted;
            _blackEngine.SearchStarted -= OnSearchStarted;
            _whiteEngine.SearchDebugInfosChanged -= OnSearchDebugInfoChanged;
            _blackEngine.SearchDebugInfosChanged -= OnSearchDebugInfoChanged;
            _whiteEngine.SearchStopped -= OnSearchStopped;
            _blackEngine.SearchStopped -= OnSearchStopped;
        }

        private void OnBoardChanged(object? sender, EventArgs eventArgs)
        {
            if (sender is IChessEngine engine)
            {
                Board = engine.Board;
            }
        }

        private void OnSearchStopped(object? sender, EventArgs eventArgs)
        {
            if (_searchingColor == ChessColor.None)
            {
                throw new InvalidOperationException($"The engine currently searching is not the right one!");
            }
            _searchCompletionSource.TrySetResult(true);
        }

        private void OnSearchDebugInfoChanged(object? sender, EventArgs eventArgs)
        {
            if (sender is IChessEngine chessEngine)
            {
                if (chessEngine == _whiteEngine)
                {
                    WhiteSearchDebugInfos = chessEngine.SearchDebugInfos;
                    return;
                }
                else if (chessEngine == _blackEngine)
                {
                    BlackSearchDebugInfos = chessEngine.SearchDebugInfos;
                    return;
                }
            }
            throw new InvalidOperationException($"The engine currently searching is not the right one!");
        }

        private void OnSearchStarted(object? sender, EventArgs eventArgs)
        {
            if (sender is IChessEngine chessEngine)
            {
                if ((chessEngine == _whiteEngine && _searchingColor == ChessColor.White) || (chessEngine == _blackEngine && _searchingColor == ChessColor.Black))
                {
                    return;
                }
            }
            throw new InvalidOperationException($"The engine currently searching is not the right one!");
        }

        private void OnEngineDisposed(object? sender, EventArgs eventArgs)
        {
            throw new ObjectDisposedException($"One of the chess engines used was disposed.");
        }

        private IChessEngine GetCurrentEngine(bool opponentInsteadOfCurrent)
        {
            return _searchingColor switch
            {
                ChessColor.White => opponentInsteadOfCurrent ? _blackEngine : _whiteEngine,
                ChessColor.Black => opponentInsteadOfCurrent ? _whiteEngine : _blackEngine,
                _ => throw new InvalidOperationException($"The engine currently searching is not the right one!")
            };
        }
    }
}
