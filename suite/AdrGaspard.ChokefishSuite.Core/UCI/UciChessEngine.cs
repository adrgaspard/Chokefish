using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.Utils;
using System.Collections.Immutable;

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

        public UciChessEngine(string executablePath, string arguments = "", string? standardInputNewLine = null)
        {
            _lock = new object();
            _transmitter = new(executablePath, arguments, standardInputNewLine);
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
            lock (_lock)
            {
                if (!_initialized)
                {
                    _initialized = true;
                    CurrentState = UciChessGuiState.WaitingForUciok;
                    _transmitter.Start();
                    _transmitter.OutputDataReceived += OnOutputReceived;
                    _transmitter.ErrorDataReceived += OnErrorReceived;
                    _transmitter.SendInputData(UciCommands.Uci);
                    _waitReadyokTaskSource.Task.Wait();
                    _transmitter.SendInputData(UciCommands.Isready);
                    _waitReadyokTaskSource.Task.Wait();
                    Initialized?.Invoke(this, EventArgs.Empty);
                }
            }
        }

        public bool SetDebug(bool value)
        {
            if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
            {
                _transmitter.SendInputData($"{UciCommands.Debug} {(value ? UciCommands.DebugArgumentOn : UciCommands.DebugArgumentOff)}");
                return true;
            }
            return false;
        }

        public bool SetOption(string optionName, object value)
        {
            if (CurrentState != UciChessGuiState.None && CurrentState != UciChessGuiState.Disposed && Options.FirstOrDefault(option => option.Name == optionName) is UciOption option && option.TrySetValue(value))
            {
                _transmitter.SendInputData($"{UciCommands.Setoption} {UciCommands.SetoptionArgumentName} {optionName} {UciCommands.SetoptionArgumentValue} {option.GetStringValue()}");
                return true;
            }
            return false;
        }

        public bool ResetGame()
        {
            if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
            {
                _transmitter.SendInputData(UciCommands.Ucinewgame);
                Board = null;
                BoardChanged?.Invoke(this, EventArgs.Empty);
                return true;
            }
            return false;
        }

        public bool SetPosition(string initialPosition, IEnumerable<string> moves, bool refreshBoard = true)
        {
            if (CurrentState is not UciChessGuiState.None and not UciChessGuiState.Disposed)
            {
                bool isStartpos = initialPosition == UciCommands.PositionArgumentStartpos;
                if (isStartpos || initialPosition.IsValidFen())
                {
                    moves ??= Enumerable.Empty<string>();
                    CurrentState = UciChessGuiState.Idling;
                    _transmitter.SendInputData($"{UciCommands.Position} {(isStartpos ? "" : $"{UciCommands.PositionArgumentFen} ")}{initialPosition}{(moves.Any() ? $" {UciCommands.PositionArgumentMoves} {string.Join(' ', moves)}" : "")}");
                    if (refreshBoard)
                    {
                        if (!RefreshBoard())
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        }

        public bool StartSearch(ChessTimeSystem searchTimeSystem)
        {
            return StartSearch(searchTimeSystem, Options.FirstOrDefault(option => option.Name == OptionHelper.PonderOptionName) is CheckOption option && option.Value);
        }

        public bool StartSearch(ChessTimeSystem searchTimeSystem, bool ponder)
        {
            if (CurrentState == UciChessGuiState.Idling && searchTimeSystem.IsValid && searchTimeSystem.ToUciString() is string uciString)
            {
                CurrentState = ponder ? UciChessGuiState.Pondering : UciChessGuiState.Searching;
                SearchResult = null;
                SearchDebugInfos = null;
                _transmitter.SendInputData($"{UciCommands.Go}{(ponder ? $" {UciCommands.GoArgumentPonder}" : "")} {uciString}");
                SearchStarted?.Invoke(this, EventArgs.Empty);
                return true;
            }
            return false;
        }

        public bool SwitchFromPonderingToClassicalSearch()
        {
            if (CurrentState == UciChessGuiState.Pondering)
            {
                CurrentState = UciChessGuiState.Searching;
                _transmitter.SendInputData(UciCommands.Ponderhit);
                return true;
            }
            return false;
        }

        public bool StopSearch()
        {
            if (CurrentState is UciChessGuiState.Pondering or UciChessGuiState.Searching)
            {
                CurrentState = UciChessGuiState.Idling;
                _transmitter.SendInputData(UciCommands.Stop);
                SearchStopped?.Invoke(this, EventArgs.Empty);
                return true;
            }
            return false;
        }

        public bool RefreshBoard()
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
            throw new ChessException($"The {nameof(UciChessEngine)} received the following error: {error}");
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
            _waitUciokTaskSource.SetResult(true);
            _transmitter.SendInputData(UciCommands.Isready);
        }

        private void ProcessReadyokResponse()
        {
            if (CurrentState == UciChessGuiState.WaitingForReadyOk)
            {
                CurrentState = UciChessGuiState.Idling;
                _waitReadyokTaskSource.SetResult(true);
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
                _refreshBoardTaskSource?.SetResult(true);
                BoardChanged?.Invoke(this, EventArgs.Empty);
            }
        }
    }
}
