using AdrGaspard.ChokefishSuite.Core.Utils;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Match
{
    public class HypothesisResultToBrushConverter : IValueConverter
    {
        private static readonly SolidColorBrush Green = new(Color.FromRgb(0x32, 0xDB, 0x4F));
        private static readonly SolidColorBrush Red = new(Color.FromRgb(0xF7, 0x2F, 0x2F));
        private static readonly SolidColorBrush LightGray = new(Color.FromRgb(0xD0, 0xD0, 0xD0));

        public object? Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is HypothesisResult result)
            {
                return result switch
                {
                    HypothesisResult.None => LightGray,
                    HypothesisResult.H0 => Red,
                    HypothesisResult.H1 => Green,
                    HypothesisResult.Inconclusive => LightGray,
                    _ => null
                };
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
