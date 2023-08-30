namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    [Flags]
    public enum ChessGameResult : byte
    {
        None = 0,
        Playing = 1,
        WhiteMated = 2,
        BlackMated = 4,
        Stalemate = 8,
        Repetition = 16,
        FiftyMoveRule = 32,
        InsufficientMaterial = 64,
        Draw = Stalemate | Repetition | FiftyMoveRule | InsufficientMaterial
    }
}
