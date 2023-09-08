using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Common
{
    public class BoolToSuccessBrushConverter : IValueConverter
    {
        private static readonly SolidColorBrush Success = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Failure = new(Color.FromRgb(0xF7, 0x2F, 0x2F));

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool inverted = parameter is bool paramBool && paramBool;
            return (value is bool valueBool && valueBool) ^ inverted ? Success : Failure;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
