using Avalonia.Media;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common
{
    public class ColorToBrushConverter : OneWayValueConverter
    {
        public override object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return new SolidColorBrush((Color)value!);
        }
    }
}
