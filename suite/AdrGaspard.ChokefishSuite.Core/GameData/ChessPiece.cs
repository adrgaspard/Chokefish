namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    public readonly struct ChessPiece
    {
        public static readonly ChessPiece NoPiece = new(ChessPieceType.None, ChessColor.None);

        public readonly ChessPieceType Type;
        public readonly ChessColor Color;

        public ChessPiece(ChessPieceType type, ChessColor color)
        {
            Type = type;
            Color = color;
        }

        public static bool operator ==(ChessPiece left, ChessPiece right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(ChessPiece left, ChessPiece right)
        {
            return !(left == right);
        }

        public override bool Equals(object? obj)
        {
            return obj is ChessPiece piece && Equals(piece);
        }

        public bool Equals(ChessPiece other)
        {
            return Type == other.Type && Color == other.Color;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Type, Color);
        }

        public override string ToString()
        {
            return $"{Color} {Type}";
        }
    }
}
