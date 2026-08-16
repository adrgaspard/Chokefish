using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Board
{
    public class PieceToImageSourceConverter : OneWayValueConverter
    {
        public const string WhiteName = "White";
        public const string BlackName = "Black";
        public const string PawnName = "Pawn";
        public const string KnightName = "Knight";
        public const string BishopName = "Bishop";
        public const string RookName = "Rook";
        public const string QueenName = "Queen";
        public const string KingName = "King";

        private static readonly Dictionary<string, Bitmap?> ImagesCache = new();

        public override object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
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
                return GetPieceImage(string.Concat(colorPart, typePart));
            }
            return null;
        }

        private static Bitmap? GetPieceImage(string name)
        {
            if (ImagesCache.TryGetValue(name, out Bitmap? bitmap))
            {
                return bitmap;
            }
            Uri uri = new($"avares://AdrGaspard.ChokefishSuite.UI.Avalonia/Resources/Sprites/Pieces/{name}.png");
            if (AssetLoader.Exists(uri))
            {
                bitmap = new Bitmap(AssetLoader.Open(uri));
                ImagesCache[name] = bitmap;
            }
            return bitmap;
        }
    }
}
