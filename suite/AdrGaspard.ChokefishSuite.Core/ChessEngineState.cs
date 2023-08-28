namespace AdrGaspard.ChokefishSuite.Core
{
    public enum ChessEngineState
    {
        None,
        WaitingForUciok,
        WaitingForReadyOk,
        Idling,
        WaitingForDisplay,
        Disposed
    }
}
