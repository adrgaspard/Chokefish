namespace AdrGaspard.ChokefishSuite.Core
{
    public static class OptionHelper
    {
        public const string Check = "check";
        public const string CheckValueTrue = "true";
        public const string CheckValueFalse = "false";

        public const string Spin = "spin";

        public static ChessEngineOption? ConvertToOption(this string[] splitedArguments)
        {
            if (splitedArguments.Length >= 4)
            {
                string nameAttribute = splitedArguments[0];
                string nameValue = splitedArguments[1];
                string typeAttribute = splitedArguments[2];
                string typeValue = splitedArguments[3];
                if (nameAttribute == Responses.OptionArgumentName && typeAttribute == Responses.OptionArgumentType)
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

        private static ChessEngineCheckOption? ConvertToCheckOption(this string[] splitedArguments, string nameValue)
        {
            if (splitedArguments.Length == 6)
            {
                string defaultAttribute = splitedArguments[4];
                string defaultBool = splitedArguments[5];
                if (defaultAttribute == Responses.OptionArgumentDefault)
                {
                    if (defaultBool == CheckValueTrue)
                    {
                        return new ChessEngineCheckOption(nameValue, true);
                    }
                    else if (defaultBool == CheckValueFalse)
                    {
                        return new ChessEngineCheckOption(nameValue, false);
                    }
                }
            }
            return null;
        }

        private static ChessEngineSpinOption? ConvertToSpinOption(this string[] splitedArguments, string nameValue)
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
                        case Responses.OptionArgumentDefault:
                            editingDefault = true;
                            break;
                        case Responses.OptionArgumentMin:
                            editingMin = true;
                            break;
                        case Responses.OptionArgumentMax:
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
                ? new ChessEngineSpinOption(nameValue, defaultInt.Value, minInt, maxInt)
                : null;
        }
    }
}
