namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    public readonly struct ChessSquare : IEquatable<ChessSquare>
    {
        public readonly int Rank;
        public readonly int File;

        public ChessSquare(int rank, int file)
        {
            Rank = rank;
            File = file;
        }

        public bool Equals(ChessSquare other)
        {
            return Rank == other.Rank && File == other.File;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Rank, File);
        }

        public override bool Equals(object? obj)
        {
            return obj is ChessSquare value && Equals(value);
        }

        public static bool operator ==(ChessSquare left, ChessSquare right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(ChessSquare left, ChessSquare right)
        {
            return !(left == right);
        }

        public static ChessSquare operator *(ChessSquare left, int right)
        {
            return new(left.Rank * right, left.File * right);
        }

        public static ChessSquare operator +(ChessSquare left, ChessSquare right)
        {
            return new(left.Rank + right.Rank, left.File + right.File);
        }

        public static ChessSquare operator -(ChessSquare left, ChessSquare right)
        {
            return new(left.Rank - right.Rank, left.File - right.File);
        }

        public static ChessSquare operator -(ChessSquare position)
        {
            return new(-position.Rank, -position.File);
        }

        public override string ToString()
        {
            return $"({Rank}, {File})";
        }
    }
}
