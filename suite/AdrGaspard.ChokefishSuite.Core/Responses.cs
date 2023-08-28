namespace AdrGaspard.ChokefishSuite.Core
{
    public static class Responses
    {
        public const string Id = "id";
        public const string IdArgumentName = "name";
        public const string IdArgumentAuthor = "author";

        public const string Uciok = "uciok";
        public const string Readyok = "readyok";

        public const string Bestmove = "bestmove";
        public const string BestmoveArgumentPonder = "ponder";

        public const string Info = "info";
        public const string InfoArgumentDepth = "depth";
        public const string InfoArgumentTime = "time";
        public const string InfoArgumentNodes = "nodes";
        public const string InfoArgumentScore = "score";
        public const string InfoArgumentScoreArgumentCp = "cp";
        public const string InfoArgumentScoreArgumentMate = "mate";

        public const string Option = "option";
        public const string OptionArgumentName = "name";
        public const string OptionArgumentType = "type";
        public const string OptionArgumentDefault = "default";
        public const string OptionArgumentMin = "min";
        public const string OptionArgumentMax = "max";

        public const string Fen = "fen";
    }
}
