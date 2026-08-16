using System.Globalization;
using Avalonia.Data.Converters;
using Avalonia.Media;
using Brushes = AdrGaspard.ChokefishSuite.UI.Avalonia.Models.Brushes;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Board
{
    public class BoolsToBrushConverter : IMultiValueConverter
    {
        public object? Convert(IList<object?> values, Type targetType, object? parameter, CultureInfo culture)
        {
            bool inverted = parameter is true;
            return values.Count == 3 && values[0] is bool blackSquare && values[1] is bool selected && values[2] is bool lastMove
                ? selected || lastMove
                    ? ((blackSquare ^ inverted) ? Brushes.HighlightedDarkColorBrush : Brushes.HighlightedLightColorBrush)
                    : ((blackSquare ^ inverted) ? Brushes.DarkColorBrush : Brushes.LightColorBrush)
                : null;
        }
    }
}
