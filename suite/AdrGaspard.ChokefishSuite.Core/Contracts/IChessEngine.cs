using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.UCI;
using System.Collections.Immutable;

namespace AdrGaspard.ChokefishSuite.Core.Contracts
{
    public interface IChessEngine : IDisposable
    {
        string? Name { get; }

        string? Author { get; }

        ImmutableList<UciOption> Options { get; }

        ChessBoard? Board { get; }

        SearchResult? SearchResult { get; }

        SearchDebugInfos? SearchDebugInfos { get; }

        event EventHandler? Initialized;

        event EventHandler? BoardChanged;

        event EventHandler? SearchStarted;

        event EventHandler? SearchDebugInfosChanged;

        event EventHandler? SearchStopped;

        event EventHandler? Disposed;

        void Initialize();

        bool SetDebug(bool value);

        bool SetOption(string optionName, object value);

        bool ResetGame();

        bool SetPosition(string initialPosition, IEnumerable<string> moves, bool refreshBoard = true);

        bool StartSearch(ChessTimeSystem searchTimeSystem);

        bool StartSearch(ChessTimeSystem searchTimeSystem, bool ponder);

        bool SwitchFromPonderingToClassicalSearch();

        bool StopSearch();

        bool RefreshBoard();
    }
}
