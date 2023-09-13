using AdrGaspard.ChokefishSuite.Core.GameData;
using System;
using System.Globalization;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Board
{
    public class ChessResultToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
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
                    ChessGameResult.Draw => "CONVERTION ERROR",
                    _ => "CONVERTION ERROR",
                }
                : (object)"CONVERTION ERROR";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
