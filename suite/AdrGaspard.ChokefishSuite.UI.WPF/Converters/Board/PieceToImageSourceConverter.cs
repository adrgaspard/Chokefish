using AdrGaspard.ChokefishSuite.Core.GameData;
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Board
{
    public class PieceToImageSourceConverter : IValueConverter
    {
        public const string WhiteName = "White";
        public const string BlackName = "Black";
        public const string PawnName = "Pawn";
        public const string KnightName = "Knight";
        public const string BishopName = "Bishop";
        public const string RookName = "Rook";
        public const string QueenName = "Queen";
        public const string KingName = "King";

        public object? Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is ChessPiece piece && piece.Color != ChessColor.None && piece.Type != ChessPieceType.None)
            {
                string colorPart = piece.Color == ChessColor.White ? WhiteName : BlackName;
                string typePart = piece.Type switch
                {
                    ChessPieceType.Pawn => PawnName,
                    ChessPieceType.Knight => KnightName,
                    ChessPieceType.Bishop => BishopName,
                    ChessPieceType.Rook => RookName,
                    ChessPieceType.Queen => QueenName,
                    ChessPieceType.King => KingName,
                    _ => ""
                };
                return Application.Current?.FindResource(string.Concat(colorPart, typePart)) as ImageSource;
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
