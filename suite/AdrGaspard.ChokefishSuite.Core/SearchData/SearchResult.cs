using AdrGaspard.ChokefishSuite.Core.GameData;

namespace AdrGaspard.ChokefishSuite.Core.SearchData
{
    public readonly struct SearchResult : IEquatable<SearchResult>
    {
        public readonly ChessMove BestMove;
        public readonly ChessMove? PonderMove;

        public SearchResult(ChessMove bestMove) : this(bestMove, null)
        {
        }

        public SearchResult(ChessMove bestMove, ChessMove? ponderMove)
        {
            BestMove = bestMove;
            PonderMove = ponderMove;
        }

        public bool Equals(SearchResult other)
        {
            return BestMove == other.BestMove && PonderMove == other.PonderMove;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(BestMove, PonderMove);
        }

        public override bool Equals(object? obj)
        {
            return obj is SearchResult other && Equals(other);
        }

        public static bool operator ==(SearchResult left, SearchResult right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(SearchResult left, SearchResult right)
        {
            return !(left == right);
        }
    }
}
