using Avalonia.Media;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common
{
    public class BoolOrIntToSuccessBrushConverter : OneWayValueConverter
    {
        private static readonly SolidColorBrush Success = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Failure = new(Color.FromRgb(0xF7, 0x2F, 0x2F));

        public override object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
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
    }
}
