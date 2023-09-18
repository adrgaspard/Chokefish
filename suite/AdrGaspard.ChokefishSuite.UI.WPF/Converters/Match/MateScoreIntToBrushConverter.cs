using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Match
{
    public class MateScoreIntToBrushConverter : IValueConverter
    {
        private static readonly SolidColorBrush Mate = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Mated = new(Color.FromRgb(0xF7, 0x2F, 0x2F));

        public object? Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is int score && score != 0 ? score > 0 ? Mate : Mated : (object?)null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
