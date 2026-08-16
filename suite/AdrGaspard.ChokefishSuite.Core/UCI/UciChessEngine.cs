using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.Utils;
using System.Collections.Immutable;
using System.Diagnostics;

namespace AdrGaspard.ChokefishSuite.Core.UCI
{
    public class UciChessEngine : IChessEngine
    {
        private static readonly ImmutableSortedDictionary<UciChessGuiState, ImmutableSortedSet<string>> _allowedResponses = new Dictionary<UciChessGuiState, ImmutableSortedSet<string>>()
        {
            { UciChessGuiState.None,                Enumerable.Empty<string>().ToImmutableSortedSet() },
            { UciChessGuiState.WaitingForUciok,     new List<string>(){ UciResponses.Id, UciResponses.Option, UciResponses.Uciok }.ToImmutableSortedSet() },
            { UciChessGuiState.WaitingForReadyOk,   new List<string>(){ UciResponses.Readyok }.ToImmutableSortedSet() },
            { UciChessGuiState.WaitingForDisplay,   new List<string>(){ UciResponses.Readyok, UciResponses.Fen }.ToImmutableSortedSet() },
            { UciChessGuiState.Idling,              new List<string>(){ UciResponses.Readyok }.ToImmutableSortedSet() },
            { UciChessGuiState.Pondering,           new List<string>(){ UciResponses.Readyok, UciResponses.Info }.ToImmutableSortedSet() },
            { UciChessGuiState.Searching,           new List<string>(){ UciResponses.Readyok, UciResponses.Info, UciResponses.Bestmove }.ToImmutableSortedSet() },
            { UciChessGuiState.Disposed,            Enumerable.Empty<string>().ToImmutableSortedSet() },
        }.ToImmutableSortedDictionary();

        private readonly object _lock;
        private readonly IOTransmitter _transmitter;
        private readonly List<UciOption> _options;
        private bool _initialized;
        private UciChessGuiState _currentState;
        private UciChessGuiState _previousState;
        private readonly TaskCompletionSource<bool> _waitUciokTaskSource;
        private readonly TaskCompletionSource<bool> _waitReadyokTaskSource;
        private TaskCompletionSource<bool>? _refreshBoardTaskSource;

        public event EventHandler? Initialized;
        public event EventHandler? BoardChanged;
        public event EventHandler? SearchStarted;
        public event EventHandler? SearchDebugInfosChanged;
        public event EventHandler? SearchStopped;
        public event EventHandler? Disposed;

        public UciChessEngine(IOTransmitter transmitter)
        {
            _lock = new object();
            _transmitter = transmitter;
            _currentState = UciChessGuiState.None;
            _previousState = UciChessGuiState.None;
            _options = new List<UciOption>();
            _waitUciokTaskSource = new();
            _waitReadyokTaskSource = new();
            Options = Enumerable.Empty<UciOption>().ToImmutableList();
        }

        public UciChessGuiState CurrentState
        {
            get => _currentState;
            private set
            {
                if (_currentState != value)
                {
                    _previousState = _currentState;
                    _currentState = value;
                }
            }
        }

        public string? Name { get; private set; }

        public string? Author { get; private set; }

        public ImmutableList<UciOption> Options { get; private set; }

        public ChessBoard? Board { get; private set; }

        public SearchResult? SearchResult { get; private set; }

        public SearchDebugInfos? SearchDebugInfos { get; private set; }

        public void Initialize()
        {
            bool hasProcessedInitialization = false;
            lock (_lock)
            {
                if (!_initialized)
                {
                    hasProcessedInitialization = true;
                    _initialized = true;
                    CurrentState = UciChessGuiState.WaitingForUciok;
                    _transmitter.Start();
                    _transmitter.OutputDataReceived += OnOutputReceived;
                    _transmitter.ErrorDataReceived += OnErrorReceived;
                    _transmitter.SendInputData(UciCommands.Uci);
                    if (!_waitUciokTaskSource.Task.Wait(TimeSpan.FromSeconds(5)))
                    {
                        throw new InvalidOperationException($"The engine didn't respond to the '{UciCommands.Uci}' command.");
                    }
                    _transmitter.SendInputData(UciCommands.Isready);
                    if (!_waitReadyokTaskSource.Task.Wait(TimeSpan.FromSeconds(5)))
                    {
                        throw new InvalidOperationException($"The engine didn't respond to the '{UciCommands.Isready}' command.");
                    }
                }
            }
            if (hasProcessedInitialization)
            {
                Initialized?.Invoke(this, EventArgs.Empty);
            }
        }

        public bool SetDebug(bool value)
        {
            lock (_lock)
            {
                if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
                {
                    _transmitter.SendInputData($"{UciCommands.Debug} {(value ? UciCommands.DebugArgumentOn : UciCommands.DebugArgumentOff)}");
                    return true;
                }
                return false;
            }
        }

        public bool SetOption(string optionName, object value)
        {
            lock (_lock)
            {
                if (CurrentState != UciChessGuiState.None && CurrentState != UciChessGuiState.Disposed && Options.FirstOrDefault(option => option.Name == optionName) is UciOption option && option.TrySetValue(value))
                {
                    _transmitter.SendInputData($"{UciCommands.Setoption} {UciCommands.SetoptionArgumentName} {optionName} {UciCommands.SetoptionArgumentValue} {option.GetStringValue()}");
                    return true;
                }
                return false;
            }
        }

        public bool ResetGame()
        {
            bool boardChanged = false;
            lock (_lock)
            {
                if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
                {
                    _transmitter.SendInputData(UciCommands.Ucinewgame);
                    Board = null;
                    boardChanged = true;
                }
            }
            if (boardChanged)
            {
                BoardChanged?.Invoke(this, EventArgs.Empty);
            }
            return boardChanged;
        }

        public bool SetPosition(string initialPosition, IEnumerable<string> moves, bool refreshBoard = true)
        {
            lock (_lock)
            {
                if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
                {
                    bool isStartpos = initialPosition == UciCommands.PositionArgumentStartpos;
                    if (isStartpos || initialPosition.IsValidFen())
                    {
                        moves ??= Enumerable.Empty<string>();
                        CurrentState = UciChessGuiState.Idling;
                        _transmitter.SendInputData($"{UciCommands.Position} {(isStartpos ? "" : $"{UciCommands.PositionArgumentFen} ")}{initialPosition}{(moves.Any() ? $" {UciCommands.PositionArgumentMoves} {string.Join(' ', moves)}" : "")}");
                        if (!refreshBoard)
                        {
                            return true;
                        }
                    }
                }
            }
            return refreshBoard && RefreshBoard();
        }

        public bool StartSearch(ChessTimeSystem searchTimeSystem)
        {
            bool searchStarted = false;
            lock (_lock)
            {
                if (CurrentState == UciChessGuiState.Idling && searchTimeSystem.IsValid && searchTimeSystem.ToUciString() is string uciString)
                {
                    CurrentState = UciChessGuiState.Searching;
                    SearchResult = null;
                    SearchDebugInfos = null;
                    _transmitter.SendInputData($"{UciCommands.Go} {uciString}");
                    searchStarted = true;
                }
            }
            if (searchStarted)
            {
                SearchStarted?.Invoke(this, EventArgs.Empty);
            }
            return searchStarted;
        }

        public bool SwitchFromPonderingToClassicalSearch()
        {
            lock (_lock)
            {
                if (CurrentState == UciChessGuiState.Pondering)
                {
                    CurrentState = UciChessGuiState.Searching;
                    _transmitter.SendInputData(UciCommands.Ponderhit);
                    return true;
                }
            }
            return false;
        }

        public bool StopSearch()
        {
            bool searchStopped = false;
            lock (_lock)
            {
                if (CurrentState is UciChessGuiState.Pondering or UciChessGuiState.Searching)
                {
                    CurrentState = UciChessGuiState.Idling;
                    _transmitter.SendInputData(UciCommands.Stop);
                    searchStopped = true;
                }
            }
            if (searchStopped)
            {
                SearchStopped?.Invoke(this, EventArgs.Empty);
            }
            return searchStopped;
        }

        public bool RefreshBoard()
        {
            lock (_lock)
            {
                if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
                {
                    CurrentState = UciChessGuiState.WaitingForDisplay;
                    _transmitter.SendInputData($"{UciCommands.Display} {UciCommands.DisplayArgumentFen}");
                    _refreshBoardTaskSource = new();
                    _refreshBoardTaskSource.Task.Wait();
                    return true;
                }
                return false;
            }
        }

        public void Dispose()
        {
            lock (_lock)
            {
                CurrentState = UciChessGuiState.Disposed;
                if (_initialized)
                {
                    _transmitter.SendInputData(UciCommands.Quit);
                    _transmitter.OutputDataReceived -= OnOutputReceived;
                    _transmitter.ErrorDataReceived -= OnErrorReceived;
                    if (_previousState is UciChessGuiState.Searching or UciChessGuiState.Pondering)
                    {
                        SearchStopped?.Invoke(this, EventArgs.Empty);
                    }
                }
                _transmitter.Dispose();
                Disposed?.Invoke(this, EventArgs.Empty);
            }
        }

        private void OnOutputReceived(object? sender, string output)
        {
            string[] splitedOutput = output.Split(' ', 2);
            string response = splitedOutput.Length > 0 ? splitedOutput[0] : "";
            string arguments = splitedOutput.Length > 1 ? splitedOutput[1] : "";
            if (_allowedResponses.TryGetValue(CurrentState, out ImmutableSortedSet<string>? allowedResponses) && allowedResponses.Contains(response))
            {
                switch (response)
                {
                    case UciResponses.Id:
                        ProcessIdResponse(arguments);
                        break;
                    case UciResponses.Option:
                        ProcessOptionResponse(arguments);
                        break;
                    case UciResponses.Uciok:
                        ProcessUciokResponse();
                        break;
                    case UciResponses.Readyok:
                        ProcessReadyokResponse();
                        break;
                    case UciResponses.Bestmove:
                        ProcessBestmoveResponse(arguments);
                        break;
                    case UciResponses.Info:
                        ProcessInfoResponse(arguments);
                        break;
                    case UciResponses.Fen:
                        ProcessFenResponse(arguments);
                        break;
                    default: break;
                }
            }
        }

        private void OnErrorReceived(object? sender, string error)
        {
            Debug.WriteLine($"[ENGINE ERROR] {error}");
            ChessException exception = new($"The {nameof(UciChessEngine)} received the following error: {error}");
            _waitUciokTaskSource.TrySetException(exception);
            _waitReadyokTaskSource.TrySetException(exception);
            _refreshBoardTaskSource?.TrySetException(exception);
        }

        private void ProcessIdResponse(string arguments)
        {
            string[] splitedArguments = arguments.Split(' ', 2);
            if (splitedArguments.Length == 2 && !string.IsNullOrWhiteSpace(splitedArguments[1]))
            {
                switch (splitedArguments[0])
                {
                    case UciResponses.IdArgumentName:
                        Name = splitedArguments[1].Trim();
                        break;
                    case UciResponses.IdArgumentAuthor:
                        Author = splitedArguments[1].Trim();
                        break;
                    default: break;
                }
            }
        }

        private void ProcessOptionResponse(string arguments)
        {
            if (arguments.Split(' ').ConvertToOption() is UciOption option)
            {
                _options.Add(option);
            }
        }

        private void ProcessUciokResponse()
        {
            CurrentState = UciChessGuiState.WaitingForReadyOk;
            Options = _options.ToImmutableList();
            _waitUciokTaskSource.TrySetResult(true);
        }

        private void ProcessReadyokResponse()
        {
            if (CurrentState == UciChessGuiState.WaitingForReadyOk)
            {
                CurrentState = UciChessGuiState.Idling;
                _waitReadyokTaskSource.TrySetResult(true);
            }
        }

        private void ProcessBestmoveResponse(string arguments)
        {
            if (arguments.ToSearchResult() is SearchResult searchResult)
            {
                CurrentState = UciChessGuiState.Idling;
                SearchResult = searchResult;
                SearchStopped?.Invoke(this, EventArgs.Empty);
            }
        }

        private void ProcessInfoResponse(string arguments)
        {
            if (arguments.ToSearchDebugInfos() is SearchDebugInfos searchDebugInfos)
            {
                SearchDebugInfos = searchDebugInfos;
                SearchDebugInfosChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        private void ProcessFenResponse(string arguments)
        {
            if (CurrentState == UciChessGuiState.WaitingForDisplay && arguments.ToChessBoard() is ChessBoard board)
            {
                Board = board;
                CurrentState = _previousState;
                _refreshBoardTaskSource?.TrySetResult(true);
                BoardChanged?.Invoke(this, EventArgs.Empty);
            }
        }
    }
}
