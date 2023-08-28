namespace AdrGaspard.ChokefishSuite.Core
{
    public class ChessEngineSpinOption : ChessEngineOption
    {
        public ChessEngineSpinOption(string name, int defaultValue, int? minValue = null, int? maxValue = null) : base(name, OptionType.Spin)
        {
            DefaultValue = defaultValue;
            Value = defaultValue;
            MinValue = minValue ?? int.MinValue;
            MaxValue = maxValue ?? int.MaxValue;
            if (defaultValue < minValue)
            {
                throw new ChessEngineException($"The default value of a {nameof(ChessEngineSpinOption)} was smaller than his min value");
            }
            if (defaultValue > maxValue)
            {
                throw new ChessEngineException($"The default value of a {nameof(ChessEngineSpinOption)} was greater than his min value");
            }
        }

        public int DefaultValue { get; private init; }

        public int MinValue { get; private init; }

        public int MaxValue { get; private init; }

        public int Value { get; internal set; }

        internal override bool TrySetValue(object value)
        {
            if (value is int i && i >= MinValue && i <= MaxValue)
            {
                Value = i;
                return true;
            }
            return false;
        }

        public override string GetStringValue()
        {
            return Value.ToString();
        }
    }
}
