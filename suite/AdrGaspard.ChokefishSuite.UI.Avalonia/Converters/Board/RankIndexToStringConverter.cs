using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Board
{
    public class RankIndexToStringConverter : OneWayValueConverter
    {
        public override object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is int i ? (i + 1).ToString() : null;
        }
    }
}
