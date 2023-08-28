using AdrGaspard.ChokefishSuite.Core.Utils;

namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    public class ChessTimeSystem
    {
        public static readonly ChessTimeSystem Infinite = new(0, 0, 0, 0, 0, 0);

        private ChessTimeSystem(uint definedSearchTimeInMs, uint whiteTimeInMs, uint blackTimeInMs, uint whiteIncrementTimeInMs, uint blackIncrementTimeInMs, uint movesToGo)
        {
            DefinedSearchTimeInMs = definedSearchTimeInMs;
            WhiteTimeInMs = whiteTimeInMs;
            BlackTimeInMs = blackTimeInMs;
            WhiteIncrementTimeInMs = whiteIncrementTimeInMs;
            BlackIncrementTimeInMs = blackIncrementTimeInMs;
            MovesToGo = movesToGo;
        }

        public uint DefinedSearchTimeInMs { get; private init; }

        public uint WhiteTimeInMs { get; private init; }

        public uint BlackTimeInMs { get; private init; }

        public uint WhiteIncrementTimeInMs { get; private init; }

        public uint BlackIncrementTimeInMs { get; private init; }

        public uint MovesToGo { get; private init; }

        public bool IsInfinite => DefinedSearchTimeInMs == 0 && WhiteTimeInMs == 0 && BlackTimeInMs == 0 && WhiteIncrementTimeInMs == 0 && BlackIncrementTimeInMs == 0 && MovesToGo == 0;

        public bool IsDefined => DefinedSearchTimeInMs != 0 && WhiteTimeInMs == 0 && BlackTimeInMs == 0 && WhiteIncrementTimeInMs == 0 && BlackIncrementTimeInMs == 0 && MovesToGo == 0;

        public bool IsIncremental => DefinedSearchTimeInMs == 0 && WhiteTimeInMs != 0 && BlackTimeInMs != 0 && MovesToGo == 0;

        public bool IsControl => DefinedSearchTimeInMs == 0 && WhiteTimeInMs != 0 && BlackTimeInMs != 0 && MovesToGo != 0;

        public bool IsValid => IsInfinite ^ IsDefined ^ IsIncremental ^ IsControl;

        public static ChessTimeSystem CreateDefined(uint timeInMs)
        {
            return timeInMs == 0
                ? throw new ChessException($"The parameter {nameof(timeInMs)} must be positive.")
                : new(timeInMs, 0, 0, 0, 0, 0);
        }

        public static ChessTimeSystem CreateIncremental(uint whiteTimeInMs, uint whiteIncrementInMs, uint blackTimeInMs, uint blackIncrementInMs)
        {
            return whiteTimeInMs == 0 || blackTimeInMs == 0
                ? throw new ChessException($"Both parameters {nameof(whiteTimeInMs)} and {nameof(blackTimeInMs)} must be positive.")
                : new(0, whiteTimeInMs, blackTimeInMs, whiteIncrementInMs, blackIncrementInMs, 0);
        }

        public static ChessTimeSystem CreateControl(uint whiteTimeInMs, uint blackTimeInMs, uint movesToGo)
        {
            return whiteTimeInMs == 0 || blackTimeInMs == 0 || movesToGo == 0
                ? throw new ChessException($"Parameters {nameof(whiteTimeInMs)}, {nameof(blackTimeInMs)} and {nameof(movesToGo)} must be positive.")
                : new(0, whiteTimeInMs, blackTimeInMs, 0, 0, movesToGo);
        }
    }
}
