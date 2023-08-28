using AdrGaspard.ChokefishSuite.Core.GameData;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class SquareHelper
    {
        public static string? ToUciString(this ChessSquare square)
        {
            return $"{(char)('a' + square.File)}{square.Rank + 1}";
        }

        public static ChessSquare? ToChessSquare(this string str)
        {
            if (str is not null && str.Length == 2 && str[0] is >= 'a' and < (char)('a' + ChessConsts.FilesCount))
            {
                if (int.TryParse(str[1].ToString(), out int result))
                {
                    if (result is >= 1 and <= ChessConsts.RanksCount)
                    {
                        int rank = result - 1;
                        int file = str[0] - 'a';
                        if (rank >= 0 && file >= 0 && rank < ChessConsts.RanksCount && file < ChessConsts.FilesCount)
                        {
                            return new(rank, file);
                        }
                    }
                }
            }
            return null;
        }
    }
}
