using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.UCI;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class SearchResultHelper
    {
        public static SearchResult? ToSearchResult(this string str)
        {
            string[] splitedArguments = str.Split(' ');
            return (splitedArguments.Length == 1 || (splitedArguments.Length == 3 && splitedArguments[1] == UciResponses.BestmoveArgumentPonder)) && splitedArguments[0].ToChessMove() is ChessMove bestMove
                ? (global::AdrGaspard.ChokefishSuite.Core.SearchData.SearchResult?)(splitedArguments.Length == 3
                    ? splitedArguments[2].ToChessMove() is ChessMove ponderMove ? new(bestMove, ponderMove) : null
                    : new(bestMove))
                : null;
        }
    }
}
