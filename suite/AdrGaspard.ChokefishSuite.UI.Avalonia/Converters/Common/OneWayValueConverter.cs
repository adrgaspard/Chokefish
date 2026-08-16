using System.Globalization;
using Avalonia.Data.Converters;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common
{
    public abstract class OneWayValueConverter : IValueConverter
    {
        public abstract object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture);

        public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
