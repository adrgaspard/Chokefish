namespace AdrGaspard.ChokefishSuite.Core.UCI
{
    public abstract class UciOption
    {
        protected UciOption(string name, UciOptionType type)
        {
            Name = name;
            Type = type;
        }

        public string Name { get; private init; }

        public UciOptionType Type { get; private init; }

        internal abstract bool TrySetValue(object value);

        public abstract string GetStringValue();
    }
}
