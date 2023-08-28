namespace AdrGaspard.ChokefishSuite.Core
{
    public class ChessEngineCheckOption : ChessEngineOption
    {
        public ChessEngineCheckOption(string name, bool defaultValue) : base(name, OptionType.Check)
        {
            DefaultValue = defaultValue;
            Value = defaultValue;
        }

        public bool DefaultValue { get; private init; }

        public bool Value { get; internal set; }
    }
}
