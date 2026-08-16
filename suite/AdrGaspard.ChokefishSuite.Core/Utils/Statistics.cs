namespace AdrGaspard.ChokefishSuite.Core.Utils
{
    public static class Statistics
    {
        private static double GetExpectedScore(double elo)
        {
            return 1 / (1 + Math.Pow(10, -elo / 400));
        }

        private static double GetLogLikelihoodRatio(uint wins, uint draws, uint losses, double elo0, double elo1)
        {
            double score0 = GetExpectedScore(elo0);
            double score1 = GetExpectedScore(elo1);
            double winLogRatio = Math.Log(score1 / score0);
            double lossLogRatio = Math.Log((1 - score1) / (1 - score0));
            double drawLogRatio = Math.Log((score1 * (1 - score1)) / (score0 * (1 - score0)));
            return wins * winLogRatio + losses * lossLogRatio + draws * drawLogRatio;
        }

        public static HypothesisResult SequentialProbabilityRatioTest(uint wins, uint draws, uint losses, double elo0, double elo1, double falsePositiveRisk, double falseNegativeRisk)
        {
            if (elo1 <= elo0 || falsePositiveRisk <= 0 || falsePositiveRisk >= 1 || falseNegativeRisk <= 0 || falseNegativeRisk >= 1)
            {
                return HypothesisResult.Inconclusive;
            }
            double logLikelihoodRatio = GetLogLikelihoodRatio(wins, draws, losses, elo0, elo1);
            double lowerBound = Math.Log(falseNegativeRisk / (1 - falsePositiveRisk));
            double upperBound = Math.Log((1 - falseNegativeRisk) / falsePositiveRisk);
            return logLikelihoodRatio > upperBound ? HypothesisResult.H1 : logLikelihoodRatio < lowerBound ? HypothesisResult.H0 : HypothesisResult.Inconclusive;
        }
    }
}
