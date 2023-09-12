using AdrGaspard.ChokefishSuite.Core.GameData;
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Board
{
    public class BoolAndPieceToVisibilityConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            return values.Length == 2 && values[0] is true && values[1] is ChessPiece piece && (parameter is true) == (piece == ChessPiece.NoPiece) ? Visibility.Visible : Visibility.Collapsed;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
