namespace AdrGaspard.ChokefishSuite.Core.SearchData
{
    public readonly struct SearchDebugInfos : IEquatable<SearchDebugInfos>
    {
        public readonly int Depth;
        public readonly TimeSpan Time;
        public readonly long Nodes;
        public readonly bool IsMate;
        public readonly int MateScore;
        public readonly double CentipawnScore;

        public SearchDebugInfos(int depth, TimeSpan time, long nodes, int mateScore) : this(depth, time, nodes, true, mateScore, 0)
        {
        }

        public SearchDebugInfos(int depth, TimeSpan time, long nodes, double centipawnScore) : this(depth, time, nodes, false, 0, centipawnScore)
        {
        }

        private SearchDebugInfos(int depth, TimeSpan time, long nodes, bool isMate, int mateScore, double centipawnScore)
        {
            Depth = depth;
            Time = time;
            Nodes = nodes;
            IsMate = isMate;
            MateScore = mateScore;
            CentipawnScore = centipawnScore;
        }

        public bool Equals(SearchDebugInfos other)
        {
            return Depth == other.Depth && Time == other.Time && Nodes == other.Nodes && MateScore == other.MateScore && CentipawnScore == other.CentipawnScore && MateScore == other.MateScore;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Depth, Time, Nodes, IsMate, MateScore, CentipawnScore);
        }

        public override bool Equals(object? obj)
        {
            return obj is SearchDebugInfos other && Equals(other);
        }

        public static bool operator ==(SearchDebugInfos left, SearchDebugInfos right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(SearchDebugInfos left, SearchDebugInfos right)
        {
            return !(left == right);
        }
    }
}
