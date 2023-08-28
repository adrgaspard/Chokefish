using AdrGaspard.ChokefishSuite.Core.GameData;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class MoveHelper
    {
        public static string? ToUciString(this ChessMove move)
        {
            return $"{move.OldSquare.ToUciString()}{move.NewSquare.ToUciString()}{(move.PromotionType != ChessPieceType.None ? $"={PieceTypeHelper.SymbolsToPieceType.First(pair => pair.Value == move.PromotionType).Key}" : "")}";
        }

        public static ChessMove? ToChessMove(this string str)
        {
            if (str is not null && str.Length >= 4 && str.Length <= 6)
            {
                if (str[..2].ToChessSquare() is ChessSquare oldSquare && str[2..4].ToChessSquare() is ChessSquare newSquare)
                {
                    if (str.Length == 4)
                    {
                        return new ChessMove(oldSquare, newSquare, ChessPieceType.None);
                    }
                    if (str.Length == 6 && str[4] != '=')
                    {
                        return null;
                    }
                    if (PieceTypeHelper.SymbolsToPieceType.TryGetValue(str[^1].ToString().ToLower()[0], out ChessPieceType pieceType))
                    {
                        return new ChessMove(oldSquare, newSquare, pieceType);
                    }
                }
            }
            return null;
        }
    }
}
