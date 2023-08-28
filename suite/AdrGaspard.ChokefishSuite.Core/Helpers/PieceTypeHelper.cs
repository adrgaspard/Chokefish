using AdrGaspard.ChokefishSuite.Core.GameData;
using System.Collections.Immutable;

namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class PieceTypeHelper
    {
        public static readonly ImmutableSortedDictionary<char, ChessPieceType> SymbolsToPieceType = new Dictionary<char, ChessPieceType>()
        {
            { 'p', ChessPieceType.Pawn },
            { 'n', ChessPieceType.Knight },
            { 'b', ChessPieceType.Bishop },
            { 'r', ChessPieceType.Rook },
            { 'q', ChessPieceType.Queen },
            { 'k', ChessPieceType.King }
        }.ToImmutableSortedDictionary();
    }
}
