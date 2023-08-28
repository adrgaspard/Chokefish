using AdrGaspard.ChokefishSuite.Core.Utils;

namespace AdrGaspard.ChokefishSuite.Core.UCI
{
    public class SpinOption : UciOption
    {
        public SpinOption(string name, int defaultValue, int? minValue = null, int? maxValue = null) : base(name, UciOptionType.Spin)
        {
            DefaultValue = defaultValue;
            Value = defaultValue;
            MinValue = minValue ?? int.MinValue;
            MaxValue = maxValue ?? int.MaxValue;
            if (defaultValue < minValue)
            {
                throw new ChessException($"The default value of a {nameof(SpinOption)} was smaller than his min value");
            }
            if (defaultValue > maxValue)
            {
                throw new ChessException($"The default value of a {nameof(SpinOption)} was greater than his min value");
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
