namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    [Flags]
    public enum ChessCastlingState : byte
    {
        None = 0,
        KingSide = 1,
        QueenSide = 2,
        BothSides = KingSide | QueenSide
    }
}
