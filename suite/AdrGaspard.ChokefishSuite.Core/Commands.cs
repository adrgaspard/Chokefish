namespace AdrGaspard.ChokefishSuite.Core
{
    public static class Commands
    {
        public const string Uci = "uci";

        public const string Debug = "debug";
        public const string DebugArgumentOn = "on";
        public const string DebugArgumentOff = "off";

        public const string Isready = "isready";
        public const string Setoption = "setoption";
        public const string SetoptionArgumentName = "name";
        public const string SetoptionArgumentValue = "value";

        public const string Ucinewgame = "ucinewgame";

        public const string Position = "position";
        public const string PositionArgumentFen = "fen";
        public const string PositionArgumentStartpos = "startpos";
        public const string PositionArgumentMoves = "moves";

        public const string Go = "go";
        public const string GoArgumentPonder = "ponder";
        public const string GoArgumentWtime = "wtime";
        public const string GoArgumentBtime = "btime";
        public const string GoArgumentWinc = "winc";
        public const string GoArgumentBinc = "binc";
        public const string GoArgumentMovestogo = "movestogo";
        public const string GoArgumentMovetime = "movetime";
        public const string GoArgumentInfinite = "infinite";

        public const string Stop = "stop";

        public const string Ponderhit = "ponderhit";

        public const string Quit = "quit";

        public const string Display = "display";
        public const string DisplayArgumentFen = "fen";
    }
}
