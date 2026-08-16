using System.Globalization;
using Avalonia.Data.Converters;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Match
{
    public class RatioToWidthConverter : IMultiValueConverter
    {
        public object Convert(IList<object?> values, Type targetType, object? parameter, CultureInfo culture)
        {
            return values.Count == 2 && values[0] is double ratio && values[1] is double actualWidth
                ? ratio * actualWidth
                : 0.0;
        }
    }
}
