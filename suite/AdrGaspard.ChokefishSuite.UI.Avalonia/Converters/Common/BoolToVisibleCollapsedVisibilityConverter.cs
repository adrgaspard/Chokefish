using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common
{
    public class BoolToVisibleCollapsedVisibilityConverter : OneWayValueConverter
    {
        public override object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is true ^ parameter is true;
        }
    }
}
