using AdrGaspard.ChokefishSuite.Core.Utils;

namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    public readonly struct ChessMove : IEquatable<ChessMove>
    {
        public readonly ChessSquare OldSquare;
        public readonly ChessSquare NewSquare;
        public readonly ChessPieceType PromotionType;

        public ChessMove(ChessSquare oldPosition, ChessSquare newPosition, ChessPieceType promotionType)
        {
            OldSquare = oldPosition;
            NewSquare = newPosition;
            if (promotionType == ChessPieceType.King)
            {
                throw new ChessException($"Can't create a ChessMove with a promotion type to {ChessPieceType.King}!");
            }
            PromotionType = promotionType;
        }

        public static bool operator ==(ChessMove left, ChessMove right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(ChessMove left, ChessMove right)
        {
            return !(left == right);
        }

        public override bool Equals(object? obj)
        {
            return obj is ChessMove movement && Equals(movement);
        }

        public bool Equals(ChessMove other)
        {
            return OldSquare == other.OldSquare && NewSquare == other.NewSquare && PromotionType == other.PromotionType;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(OldSquare, NewSquare, PromotionType);
        }

        public override string ToString()
        {
            return $"Old: {OldSquare}, New: {NewSquare}, {nameof(PromotionType)}: {PromotionType}";
        }
    }
}
