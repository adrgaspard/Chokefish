using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Match
{
    public class MateScoreIntToStringConverter : OneWayValueConverter
    {
        public override object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is int score && score != 0 ? score > 0 ? $"Mate in {score}" : $"Mated in {score}" : "CONVERSION ERROR";
        }
    }
}
