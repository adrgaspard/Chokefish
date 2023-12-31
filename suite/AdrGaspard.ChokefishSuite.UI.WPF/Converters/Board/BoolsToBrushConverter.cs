﻿using AdrGaspard.ChokefishSuite.UI.WPF.Models;
using System;
using System.Globalization;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Board
{
    public class BoolsToBrushConverter : IMultiValueConverter
    {
        public object? Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            bool inverted = parameter is true;
            return values.Length == 3 && values[0] is bool blackSquare && values[1] is bool selected && values[2] is bool lastMove
                ? selected || lastMove
                    ? ((blackSquare ^ inverted) ? Brushes.HighlightedDarkColorBrush : Brushes.HighlightedLightColorBrush)
                    : ((blackSquare ^ inverted) ? Brushes.DarkColorBrush : Brushes.LightColorBrush)
                : null;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
