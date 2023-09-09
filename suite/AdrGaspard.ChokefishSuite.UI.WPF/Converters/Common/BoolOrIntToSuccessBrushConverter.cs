using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Common
{
    public class BoolOrIntToSuccessBrushConverter : IValueConverter
    {
        private static readonly SolidColorBrush Success = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Failure = new(Color.FromRgb(0xF7, 0x2F, 0x2F));

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool inverted = parameter is bool paramBool && paramBool;
            bool boolValue = value switch
            {
                int valueInt => valueInt == 0,
                bool valueBool => valueBool,
                _ => false
            };
            return boolValue ^ inverted ? Success : Failure;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
