using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Board
{
    public class ChessResultToStringConverter : OneWayValueConverter
    {
        public override object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is ChessGameResult result
                ? result switch
                {
                    ChessGameResult.None => " ",
                    ChessGameResult.Playing => " ",
                    ChessGameResult.WhiteMated => "Black won by checkmate!",
                    ChessGameResult.BlackMated => "White won by checkmate!",
                    ChessGameResult.Stalemate => "Draw by stalemate!",
                    ChessGameResult.Repetition => "Draw by repetition!",
                    ChessGameResult.FiftyMoveRule => "Draw by fifty moves rule!",
                    ChessGameResult.InsufficientMaterial => "Draw by insufficient material!",
                    ChessGameResult.Draw => "CONVERSION ERROR",
                    _ => "CONVERSION ERROR",
                }
                : "CONVERSION ERROR";
        }
    }
}
