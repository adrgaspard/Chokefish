using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common
{
    public class StringToSanitizedStringConverter : OneWayValueConverter
    {
        public override object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is string str
                ? (string.IsNullOrWhiteSpace(str) ? "---" : str.Trim())
                : (value is null ? "---" : "CONVERSION ERROR");
        }
    }
}
