﻿using AdrGaspard.ChokefishSuite.Core.GameData;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Converters.Board
{
    public class ChessPiecePromotionToBoolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return parameter is ChessPieceType requiredPieceType && value is ChessPieceType actualPieceType && actualPieceType == requiredPieceType;
        }

        public object? ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return parameter is ChessPieceType requiredPieceType && value is true
                ? requiredPieceType
                : null;
        }
    }
}
