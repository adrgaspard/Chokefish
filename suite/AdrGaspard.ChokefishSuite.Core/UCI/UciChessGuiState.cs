namespace AdrGaspard.ChokefishSuite.Core.UCI
{
    public enum UciChessGuiState
    {
        None,
        WaitingForUciok,
        WaitingForReadyOk,
        Idling,
        Pondering,
        Searching,
        WaitingForDisplay,
        Disposed
    }
}
