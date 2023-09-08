using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdrGaspard.ChokefishSuite.Core.Utils
{
    public static class Statistics
    {
        public enum Hypothesis
        {
            H0,
            H1,
            Inconclusive
        }

        private static double GetLikelihood(this double rating)
        {
            return 1 / (1 + Math.Pow(10, -rating / 400));
        }

        private static double ComputeLogLikelihoodRatio(uint winsCount, uint drawsCount, uint lossesCount, double elo0, double elo1)
        {
            if (winsCount == 0 || drawsCount == 0 || lossesCount == 0)
            {
                return 0;
            }
            double wins = winsCount;
            double draws = drawsCount;
            double losses = lossesCount;
            double totalGames = wins + draws + losses;
            double winProbability = wins / totalGames;
            double drawProbability = draws / totalGames;
            double averagePerformance = winProbability + drawProbability / 2;
            double squaresAverage = winProbability + drawProbability / 4;
            double variance = squaresAverage - Math.Pow(averagePerformance, 2);
            double scaledVariance = variance / totalGames;
            double likelihood0 = elo0.GetLikelihood();
            double likelihood1 = elo1.GetLikelihood();
            return (likelihood1 - likelihood0) * (2 * averagePerformance - likelihood0 - likelihood1) / (scaledVariance * 2);
        }

        public static Hypothesis SequentialProbabilityRatioTest(uint wins, uint draws, uint losses, double elo0, double elo1, double falsePositiveRisk, double falseNegativeRisk)
        {
            double logAlpha = Math.Log(falseNegativeRisk / (1 - falsePositiveRisk));
            double logBeta = Math.Log((1 - falseNegativeRisk) / falsePositiveRisk);
            double likelihoodRatio = ComputeLogLikelihoodRatio(wins, draws, losses, elo0, elo1);
            if (likelihoodRatio > logBeta)
            {
                return Hypothesis.H1;
            }
            else if (likelihoodRatio < logAlpha)
            {
                return Hypothesis.H0;
            }
            else
            {
                return Hypothesis.Inconclusive;
            }
        }
    }
}
