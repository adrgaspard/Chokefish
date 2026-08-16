using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using Avalonia.Media;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Match
{
    public class MateScoreIntToBrushConverter : OneWayValueConverter
    {
        private static readonly SolidColorBrush Mate = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Mated = new(Color.FromRgb(0xF7, 0x2F, 0x2F));

        public override object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is int score && score != 0 ? score > 0 ? Mate : Mated : null;
        }
    }
}
