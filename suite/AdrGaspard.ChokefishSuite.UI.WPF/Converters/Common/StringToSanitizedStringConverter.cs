﻿using System;
using System.Globalization;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Common
{
    public class StringToSanitizedStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is string str
                ? (string.IsNullOrWhiteSpace(str) ? "---" : str.Trim())
                : (value is null ? "---" : "CONVERTION ERROR");
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
