using AdrGaspard.ChokefishSuite.Core.SearchData;
using AdrGaspard.ChokefishSuite.Core.UCI;
using System.Globalization;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class SearchDebugInfosHelper
    {
        private enum SearchDebugInfoValue
        {
            None,
            Depth,
            Time,
            Nodes,
            CentipawnScore,
            MateScore
        }

        public static SearchDebugInfos? ToSearchDebugInfos(this string str)
        {
            string[] splitedArguments = str.Split(' ');
            SearchDebugInfoValue expectedValue = SearchDebugInfoValue.None;
            bool expectingKeyword = true, expectingScoreKeyword = false;
            int? depth = null, mateScore = null;
            TimeSpan? time = null;
            long? nodes = null;
            bool? isMate = null;
            double? centipawnScore = null;
            for (int i = 0; i < splitedArguments.Length; i++)
            {
                if (expectingKeyword)
                {
                    if (expectingScoreKeyword)
                    {
                        switch (splitedArguments[i])
                        {
                            case UciResponses.InfoArgumentScoreArgumentCp:
                                isMate = false;
                                expectedValue = SearchDebugInfoValue.CentipawnScore;
                                break;
                            case UciResponses.InfoArgumentScoreArgumentMate:
                                isMate = true;
                                expectedValue = SearchDebugInfoValue.MateScore;
                                break;
                            default: return null;
                        }
                        expectingKeyword = false;
                        expectingScoreKeyword = false;
                    }
                    else
                    {
                        switch (splitedArguments[i])
                        {
                            case UciResponses.InfoArgumentDepth:
                                expectingKeyword = false;
                                expectedValue = SearchDebugInfoValue.Depth;
                                break;
                            case UciResponses.InfoArgumentTime:
                                expectingKeyword = false;
                                expectedValue = SearchDebugInfoValue.Time;
                                break;
                            case UciResponses.InfoArgumentNodes:
                                expectingKeyword = false;
                                expectedValue = SearchDebugInfoValue.Nodes;
                                break;
                            case UciResponses.InfoArgumentScore:
                                expectingKeyword = true;
                                expectingScoreKeyword = true;
                                break;
                            default: return null;
                        }
                    }
                }
                else
                {
                    switch (expectedValue)
                    {
                        case SearchDebugInfoValue.Depth:
                            if (int.TryParse(splitedArguments[i], out int convertedDepth))
                            {
                                depth = convertedDepth;
                            }
                            else
                            {
                                return null;
                            }
                            break;
                        case SearchDebugInfoValue.Time:
                            if (long.TryParse(splitedArguments[i], out long convertedTime))
                            {
                                time = TimeSpan.FromMilliseconds(convertedTime);
                            }
                            else
                            {
                                return null;
                            }
                            break;
                        case SearchDebugInfoValue.Nodes:
                            if (long.TryParse(splitedArguments[i], out long convertedNodes))
                            {
                                nodes = convertedNodes;
                            }
                            else
                            {
                                return null;
                            }
                            break;
                        case SearchDebugInfoValue.CentipawnScore:
                            if (double.TryParse(splitedArguments[i], NumberStyles.Number, CultureInfo.InvariantCulture, out double convertedCentipawnScore))
                            {
                                centipawnScore = convertedCentipawnScore;
                            }
                            else
                            {
                                return null;
                            }
                            break;
                        case SearchDebugInfoValue.MateScore:
                            if (int.TryParse(splitedArguments[i], out int convertedMateScore))
                            {
                                mateScore = convertedMateScore;
                            }
                            else
                            {
                                return null;
                            }
                            break;
                    }
                    expectingKeyword = true;
                    expectingScoreKeyword = false;
                }
            }
            return expectingKeyword
                ? (isMate.HasValue && isMate.Value) ? new SearchDebugInfos(depth, time, nodes, mateScore) : new SearchDebugInfos(depth, time, nodes, centipawnScore)
                : null;
        }
    }
}
