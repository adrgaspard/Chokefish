using AdrGaspard.ChokefishSuite.Core.GameData;
using System.Globalization;
using Avalonia.Data.Converters;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Board
{
    public class BoolAndPieceToVisibilityConverter : IMultiValueConverter
    {
        public object Convert(IList<object?> values, Type targetType, object? parameter, CultureInfo culture)
        {
            return values.Count == 2 && values[0] is true && values[1] is ChessPiece piece && (parameter is true) == (piece == ChessPiece.NoPiece);
        }
    }
}
