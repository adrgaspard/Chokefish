using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.UCI;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class SearchResultHelper
    {
        public static SearchResult? ToSearchResult(this string str)
        {
            string[] splitedArguments = str.Split(' ').Where(part => !string.IsNullOrWhiteSpace(part)).ToArray();
            if (splitedArguments.Length == 1)
            {
                return splitedArguments[0] == UciResponses.BestmoveArgumentNullMove
                    ? new SearchResult(null)
                    : splitedArguments[0].ToChessMove() is ChessMove bestMove ? new SearchResult(bestMove) : null;
            }
            if (splitedArguments.Length == 3 && splitedArguments[1] == UciResponses.BestmoveArgumentPonder && splitedArguments[0].ToChessMove() is ChessMove bestMoveWithPonder)
            {
                return splitedArguments[2].ToChessMove() is ChessMove ponderMove ? new SearchResult(bestMoveWithPonder, ponderMove) : null;
            }
            return null;
        }
    }
}
