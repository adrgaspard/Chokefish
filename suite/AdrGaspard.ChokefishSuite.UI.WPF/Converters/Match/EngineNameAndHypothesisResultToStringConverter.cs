﻿using AdrGaspard.ChokefishSuite.Core.Utils;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Match
{
    public class EngineNameAndHypothesisResultToStringConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if (values.Length == 2 && values[0] is string engineName && values[1] is HypothesisResult result)
            {
                return result switch
                {
                    HypothesisResult.None => "Not done yet",
                    HypothesisResult.H0 => $"{engineName} is not better",
                    HypothesisResult.H1 => $"{engineName} is better",
                    HypothesisResult.Inconclusive => "Not conclusive",
                    _ => "CONVERTION ERROR",
                };
            }
            return "CONVERTION ERROR";
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
