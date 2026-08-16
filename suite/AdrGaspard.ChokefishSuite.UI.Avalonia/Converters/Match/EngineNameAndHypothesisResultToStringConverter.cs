using AdrGaspard.ChokefishSuite.Core.Utils;
using System.Globalization;
using Avalonia.Data.Converters;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Converters.Match
{
    public class EngineNameAndHypothesisResultToStringConverter : IMultiValueConverter
    {
        public object Convert(IList<object?> values, Type targetType, object? parameter, CultureInfo culture)
        {
            return values.Count == 2 && values[0] is string engineName && values[1] is HypothesisResult result
                ? result switch
                {
                    HypothesisResult.None => "Not done yet",
                    HypothesisResult.H0 => $"{engineName} is not better",
                    HypothesisResult.H1 => $"{engineName} is better",
                    HypothesisResult.Inconclusive => "Not conclusive",
                    _ => "CONVERSION ERROR",
                }
                : "CONVERSION ERROR";
        }
    }
}
