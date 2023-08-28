using AdrGaspard.ChokefishSuite.Core.Helpers;

namespace AdrGaspard.ChokefishSuite.Core.UCI
{
    public class CheckOption : UciOption
    {
        public CheckOption(string name, bool defaultValue) : base(name, UciOptionType.Check)
        {
            DefaultValue = defaultValue;
            Value = defaultValue;
        }

        public bool DefaultValue { get; private init; }

        public bool Value { get; internal set; }

        internal override bool TrySetValue(object value)
        {
            if (value is bool b)
            {
                Value = b;
                return true;
            }
            return false;
        }

        public override string GetStringValue()
        {
            return Value ? OptionHelper.CheckValueTrue : OptionHelper.CheckValueFalse;
        }
    }
}
