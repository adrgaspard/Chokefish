using System.Collections.Immutable;

namespace AdrGaspard.ChokefishSuite.Core
{
    public class ChessEngine : IDisposable
    {
        private static readonly ImmutableSortedDictionary<ChessEngineState, ImmutableSortedSet<string>> _allowedResponses = new Dictionary<ChessEngineState, ImmutableSortedSet<string>>()
        {
            { ChessEngineState.None,                Enumerable.Empty<string>().ToImmutableSortedSet() },
            { ChessEngineState.WaitingForUciok,     new List<string>(){ Responses.Id, Responses.Option, Responses.Uciok }.ToImmutableSortedSet() },
            { ChessEngineState.WaitingForReadyOk,   new List<string>(){ Responses.Readyok }.ToImmutableSortedSet() },
            { ChessEngineState.WaitingForDisplay,   new List<string>(){ Responses.Readyok, Responses.Fen }.ToImmutableSortedSet() },
            { ChessEngineState.Idling,              new List<string>(){ Responses.Readyok }.ToImmutableSortedSet() },
            { ChessEngineState.Disposed,            Enumerable.Empty<string>().ToImmutableSortedSet() },
        }.ToImmutableSortedDictionary();

        private readonly object _lock;
        private readonly IOTransmitter _transmitter;
        private bool _initialized;
        private ChessEngineState _currentState;
        private ChessEngineState _previousState;
        private readonly List<ChessEngineOption> _options;

        public ChessEngine(string executablePath)
        {
            _lock = new object();
            _transmitter = new(executablePath);
            _currentState = ChessEngineState.None;
            _previousState = ChessEngineState.None;
            _options = new List<ChessEngineOption>();
            Options = Enumerable.Empty<ChessEngineOption>().ToImmutableList();
        }

        public ChessEngineState CurrentState
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

        public ImmutableList<ChessEngineOption> Options { get; private set; }

        public void Initialize()
        {
            lock (_lock)
            {
                if (!_initialized)
                {
                    _initialized = true;
                    CurrentState = ChessEngineState.WaitingForUciok;
                    _transmitter.Start();
                    _transmitter.OutputDataReceived += OnOutputReceived;
                    _transmitter.ErrorDataReceived += OnErrorReceived;
                    _transmitter.SendInputData(Commands.Uci);
                }
            }
        }

        public bool SetDebug(bool value)
        {
            if (CurrentState != ChessEngineState.None && CurrentState != ChessEngineState.Disposed)
            {
                _transmitter.SendInputData($"{Commands.Debug} {(value ? Commands.DebugArgumentOn : Commands.DebugArgumentOff)}");
                return true;
            }
            return false;
        }

        public bool SetOption(string optionName, object value)
        {
            if (CurrentState != ChessEngineState.None && CurrentState != ChessEngineState.Disposed && Options.FirstOrDefault(option => option.Name == optionName) is ChessEngineOption option && option.TrySetValue(value))
            {
                _transmitter.SendInputData($"{Commands.Setoption} {Commands.SetoptionArgumentName} {optionName} {Commands.SetoptionArgumentValue} {option.GetStringValue()}");
                return true;
            }
            return false;
        }

        public bool ResetGame()
        {
            if (CurrentState != ChessEngineState.None && CurrentState != ChessEngineState.Disposed)
            {
                _transmitter.SendInputData(Commands.Ucinewgame);
                return true;
            }
            return false;
        }

        public bool SetPosition(string initialPosition, IEnumerable<string> moves)
        {
            if (CurrentState != ChessEngineState.None && CurrentState != ChessEngineState.Disposed)
            {
                bool isStartpos = initialPosition == Commands.PositionArgumentStartpos;
                if (isStartpos /*isValid*/) // TODO : check if initial position is valid
                {
                    moves ??= Enumerable.Empty<string>();
                    CurrentState = ChessEngineState.Idling;
                    _transmitter.SendInputData($"{Commands.Position} {(isStartpos ? "" : $"{Commands.PositionArgumentFen} ")}{initialPosition}{(moves.Any() ? $" {Commands.PositionArgumentMoves} {string.Join(' ', moves)}" : "")}");
                    return true;
                }
            }
            return false;
        }

        // TODO : start search / go command

        // TODO : stop search / stop command

        // TODO : switch from ponder to classical search / ponderhit command

        // TODO : RefreshBoard / display command

        public void Dispose()
        {
            lock (_lock)
            {
                CurrentState = ChessEngineState.Disposed;
                if (_initialized)
                {
                    _transmitter.SendInputData(Commands.Quit);
                    _transmitter.OutputDataReceived -= OnOutputReceived;
                    _transmitter.ErrorDataReceived -= OnErrorReceived;
                }
                _transmitter.Dispose();
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
                    case Responses.Id:
                        ProcessIdResponse(arguments);
                        break;
                    case Responses.Option:
                        ProcessOptionResponse(arguments);
                        break;
                    case Responses.Uciok:
                        ProcessUciokResponse();
                        break;
                    case Responses.Readyok:
                        ProcessReadyokResponse();
                        break;
                    case Responses.Bestmove:
                        // TODO
                        break;
                    case Responses.Info:
                        // TODO
                        break;
                    case Responses.Fen:
                        ProcessFenResponse(arguments);
                        break;
                    default: break;
                }
            }
        }

        private void OnErrorReceived(object? sender, string error)
        {
            throw new ChessEngineException($"The {nameof(ChessEngine)} received the following error: {error}");
        }

        private void ProcessIdResponse(string arguments)
        {
            string[] splitedArguments = arguments.Split(' ', 2);
            if (splitedArguments.Length == 2 && !string.IsNullOrWhiteSpace(splitedArguments[1]))
            {
                switch (splitedArguments[0])
                {
                    case Responses.IdArgumentName:
                        Name = splitedArguments[1].Trim();
                        break;
                    case Responses.IdArgumentAuthor:
                        Author = splitedArguments[1].Trim();
                        break;
                    default: break;
                }
            }
        }

        private void ProcessOptionResponse(string arguments)
        {
            if (arguments.Split(' ').ConvertToOption() is ChessEngineOption option)
            {
                _options.Add(option);
            }
        }

        private void ProcessUciokResponse()
        {
            CurrentState = ChessEngineState.WaitingForReadyOk;
            Options = _options.ToImmutableList();
            _transmitter.SendInputData(Commands.Isready);
        }

        private void ProcessReadyokResponse()
        {
            if (CurrentState == ChessEngineState.WaitingForReadyOk)
            {
                CurrentState = ChessEngineState.Idling;
            }
        }

        private void ProcessFenResponse(string arguments)
        {
            // TODO : actualize position
            // TODO : CurrentState = PreviousState; if position is valid
        }
    }
}
