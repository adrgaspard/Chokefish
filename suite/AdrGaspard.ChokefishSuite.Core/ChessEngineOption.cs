namespace AdrGaspard.ChokefishSuite.Core
{
    public abstract class ChessEngineOption
    {
        protected ChessEngineOption(string name, OptionType type)
        {
            Name = name;
            Type = type;
        }

        public string Name { get; private init; }

        public OptionType Type { get; private init; }

        internal abstract bool TrySetValue(object value);

        public abstract string GetStringValue();
    }
}
