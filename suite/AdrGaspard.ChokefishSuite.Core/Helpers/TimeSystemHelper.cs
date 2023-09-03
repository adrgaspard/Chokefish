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
                ? $"{UciCommands.GoArgumentMovetime} {timeSystem.DefinedSearchTimeInMs.TotalMilliseconds}"
                : timeSystem.IsIncremental
                ? $"{UciCommands.GoArgumentWtime} {timeSystem.WhiteTimeInMs.TotalMilliseconds} {UciCommands.GoArgumentBtime} {timeSystem.BlackTimeInMs.TotalMilliseconds} {UciCommands.GoArgumentWinc} {timeSystem.WhiteIncrementTimeInMs.TotalMilliseconds} {UciCommands.GoArgumentBinc} {timeSystem.BlackIncrementTimeInMs.TotalMilliseconds}"
                : timeSystem.IsControl
                ? $"{UciCommands.GoArgumentWtime} {timeSystem.WhiteTimeInMs.TotalMilliseconds} {UciCommands.GoArgumentBtime} {timeSystem.BlackTimeInMs.TotalMilliseconds} {UciCommands.GoArgumentMovestogo} {timeSystem.MovesToGo}"
                : null;
        }
    }
}
