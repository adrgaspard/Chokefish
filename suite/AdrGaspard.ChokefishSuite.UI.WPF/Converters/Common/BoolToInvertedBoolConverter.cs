﻿using System;
using System.Globalization;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Common
{
    public class BoolToInvertedBoolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is not true;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
