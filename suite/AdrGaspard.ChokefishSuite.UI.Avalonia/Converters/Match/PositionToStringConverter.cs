using AdrGaspard.ChokefishSuite.MVVM.Models;
using AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Common;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Match
{
    public class PositionToStringConverter : OneWayValueConverter
    {
        public override object Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
        {
            return value is Position position ? position.Name : "---";
        }
    }
}
