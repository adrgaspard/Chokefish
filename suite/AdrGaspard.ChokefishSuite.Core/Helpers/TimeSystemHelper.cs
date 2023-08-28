using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.UCI;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class TimeSystemHelper
    {
        public static string? ToUciString(this ChessTimeSystem timeSystem)
        {
            return timeSystem == null || !timeSystem.IsValid
                ? null
                : timeSystem.IsInfinite
                ? UciCommands.GoArgumentInfinite
                : timeSystem.IsDefined
                ? $"{UciCommands.GoArgumentMovetime} {timeSystem.DefinedSearchTimeInMs}"
                : timeSystem.IsIncremental
                ? $"{UciCommands.GoArgumentWtime} {timeSystem.WhiteTimeInMs} {UciCommands.GoArgumentBtime} {timeSystem.BlackTimeInMs} {UciCommands.GoArgumentWinc} {timeSystem.WhiteIncrementTimeInMs} {UciCommands.GoArgumentBinc} {timeSystem.BlackIncrementTimeInMs}"
                : timeSystem.IsControl
                ? $"{UciCommands.GoArgumentWtime} {timeSystem.WhiteTimeInMs} {UciCommands.GoArgumentBtime} {timeSystem.BlackTimeInMs} {UciCommands.GoArgumentMovestogo} {timeSystem.MovesToGo}"
                : null;
        }
    }
}
