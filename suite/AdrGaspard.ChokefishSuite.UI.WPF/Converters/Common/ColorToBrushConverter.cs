using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Common
{
    public class ColorToBrushConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return new SolidColorBrush((Color)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
