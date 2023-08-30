using AdrGaspard.ChokefishSuite.Core.Utils;
using static System.TimeSpan;

namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    public class ChessTimeSystem
    {
        public static readonly ChessTimeSystem Infinite = new(Zero, Zero, Zero, Zero, Zero, 0);

        private ChessTimeSystem(TimeSpan definedSearchTimeInMs, TimeSpan whiteTimeInMs, TimeSpan blackTimeInMs, TimeSpan whiteIncrementTimeInMs, TimeSpan blackIncrementTimeInMs, uint movesToGo)
        {
            DefinedSearchTimeInMs = definedSearchTimeInMs;
            WhiteTimeInMs = whiteTimeInMs;
            BlackTimeInMs = blackTimeInMs;
            WhiteIncrementTimeInMs = whiteIncrementTimeInMs;
            BlackIncrementTimeInMs = blackIncrementTimeInMs;
            MovesToGo = movesToGo;
        }

        public TimeSpan DefinedSearchTimeInMs { get; private init; }

        public TimeSpan WhiteTimeInMs { get; private init; }

        public TimeSpan BlackTimeInMs { get; private init; }

        public TimeSpan WhiteIncrementTimeInMs { get; private init; }

        public TimeSpan BlackIncrementTimeInMs { get; private init; }

        public uint MovesToGo { get; private init; }

        public bool IsInfinite => DefinedSearchTimeInMs <= Zero && WhiteTimeInMs <= Zero && BlackTimeInMs <= Zero && WhiteIncrementTimeInMs <= Zero && BlackIncrementTimeInMs <= Zero && MovesToGo == 0;

        public bool IsDefined => DefinedSearchTimeInMs != Zero && WhiteTimeInMs <= Zero && BlackTimeInMs <= Zero && WhiteIncrementTimeInMs <= Zero && BlackIncrementTimeInMs <= Zero && MovesToGo == 0;

        public bool IsIncremental => DefinedSearchTimeInMs <= Zero && WhiteTimeInMs != Zero && BlackTimeInMs != Zero && MovesToGo == 0;

        public bool IsControl => DefinedSearchTimeInMs <= Zero && WhiteTimeInMs != Zero && BlackTimeInMs != Zero && MovesToGo != 0;

        public bool IsValid => IsInfinite ^ IsDefined ^ IsIncremental ^ IsControl;

        public static ChessTimeSystem CreateDefined(TimeSpan timeInMs)
        {
            return timeInMs <= Zero
                ? throw new ChessException($"The parameter {nameof(timeInMs)} must be positive.")
                : new(timeInMs, Zero, Zero, Zero, Zero, 0);
        }

        public static ChessTimeSystem CreateIncremental(TimeSpan whiteTimeInMs, TimeSpan whiteIncrementInMs, TimeSpan blackTimeInMs, TimeSpan blackIncrementInMs)
        {
            return whiteTimeInMs <= Zero || blackTimeInMs <= Zero
                ? throw new ChessException($"Both parameters {nameof(whiteTimeInMs)} and {nameof(blackTimeInMs)} must be positive.")
                : new(Zero, whiteTimeInMs, blackTimeInMs, whiteIncrementInMs, blackIncrementInMs, 0);
        }

        public static ChessTimeSystem CreateControl(TimeSpan whiteTimeInMs, TimeSpan blackTimeInMs, uint movesToGo)
        {
            return whiteTimeInMs <= Zero || blackTimeInMs <= Zero || movesToGo == 0
                ? throw new ChessException($"Parameters {nameof(whiteTimeInMs)}, {nameof(blackTimeInMs)} and {nameof(movesToGo)} must be positive.")
                : new(Zero, whiteTimeInMs, blackTimeInMs, Zero, Zero, movesToGo);
        }
    }
}
