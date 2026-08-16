using AdrGaspard.ChokefishSuite.Core.Utils;
using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using Avalonia.Media;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Match
{
    public class HypothesisResultToBrushConverter : OneWayValueConverter
    {
        private static readonly SolidColorBrush Green = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Red = new(Color.FromRgb(0xF7, 0x2F, 0x2F));
        private static readonly SolidColorBrush LightGray = new(Color.FromRgb(0xD0, 0xD0, 0xD0));

        public override object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is HypothesisResult result
                ? result switch
                {
                    HypothesisResult.None => LightGray,
                    HypothesisResult.H0 => Red,
                    HypothesisResult.H1 => Green,
                    HypothesisResult.Inconclusive => LightGray,
                    _ => null
                }
                : null;
        }
    }
}
