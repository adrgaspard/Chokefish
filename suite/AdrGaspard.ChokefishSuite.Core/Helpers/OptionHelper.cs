using AdrGaspard.ChokefishSuite.Core.UCI;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class OptionHelper
    {
        public const string Check = "check";
        public const string CheckValueTrue = "true";
        public const string CheckValueFalse = "false";

        public const string Spin = "spin";

        public const string PonderOptionName = "Ponder";
        public const string OwnBookOptionName = "OwnBook";
        public const string ThreadsOptionName = "Threads";

        public static UciOption? ConvertToOption(this string[] splitedArguments)
        {
            if (splitedArguments.Length >= 4)
            {
                string nameAttribute = splitedArguments[0];
                string nameValue = splitedArguments[1];
                string typeAttribute = splitedArguments[2];
                string typeValue = splitedArguments[3];
                if (nameAttribute == UciResponses.OptionArgumentName && typeAttribute == UciResponses.OptionArgumentType)
                {
                    switch (typeValue)
                    {
                        case Check:
                            return splitedArguments.ConvertToCheckOption(nameValue);
                        case Spin:
                            return splitedArguments.ConvertToSpinOption(nameValue);
                        default: break;
                    }
                }
            }
            return null;
        }

        private static CheckOption? ConvertToCheckOption(this string[] splitedArguments, string nameValue)
        {
            if (splitedArguments.Length == 6)
            {
                string defaultAttribute = splitedArguments[4];
                string defaultBool = splitedArguments[5];
                if (defaultAttribute == UciResponses.OptionArgumentDefault)
                {
                    if (defaultBool == CheckValueTrue)
                    {
                        return new CheckOption(nameValue, true);
                    }
                    else if (defaultBool == CheckValueFalse)
                    {
                        return new CheckOption(nameValue, false);
                    }
                }
            }
            return null;
        }

        private static SpinOption? ConvertToSpinOption(this string[] splitedArguments, string nameValue)
        {
            bool expectingKeyword = true, errorEncountered = false, editingDefault = false, editingMin = false, editingMax = false;
            int? defaultInt = null, minInt = null, maxInt = null;
            for (int i = 4; i < splitedArguments.Length; i++)
            {
                if (errorEncountered)
                {
                    break;
                }
                if (expectingKeyword)
                {
                    editingDefault = false;
                    editingMin = false;
                    editingMax = false;
                    switch (splitedArguments[i])
                    {
                        case UciResponses.OptionArgumentDefault:
                            editingDefault = true;
                            break;
                        case UciResponses.OptionArgumentMin:
                            editingMin = true;
                            break;
                        case UciResponses.OptionArgumentMax:
                            editingMax = true;
                            break;
                        default:
                            errorEncountered = true;
                            break;
                    }
                    expectingKeyword = false;
                }
                else
                {
                    if (int.TryParse(splitedArguments[i], out int intValue))
                    {
                        if (editingDefault)
                        {
                            errorEncountered |= defaultInt.HasValue;
                            defaultInt = intValue;
                        }
                        else if (editingMin)
                        {
                            errorEncountered |= minInt.HasValue;
                            minInt = intValue;
                        }
                        else if (editingMax)
                        {
                            errorEncountered |= maxInt.HasValue;
                            maxInt = intValue;
                        }
                    }
                    else
                    {
                        errorEncountered = true;
                    }
                    expectingKeyword = true;
                }
            }
            return expectingKeyword && !errorEncountered && defaultInt.HasValue && defaultInt >= (minInt ?? int.MinValue) && defaultInt <= (maxInt ?? int.MaxValue)
                ? new SpinOption(nameValue, defaultInt.Value, minInt, maxInt)
                : null;
        }
    }
}
