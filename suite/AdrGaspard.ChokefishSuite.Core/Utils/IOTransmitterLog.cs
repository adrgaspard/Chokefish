namespace AdrGaspard.ChokefishSuite.Core.Utils
{
    public enum IOTransmitterLogKind
    {
        Sent,
        Received,
        ReceivedInfo,
        Error
    }

    public sealed class IOTransmitterLog
    {
        public IOTransmitterLog(string text, IOTransmitterLogKind kind)
        {
            Text = text;
            Kind = kind;
        }

        public string Text { get; private init; }

        public IOTransmitterLogKind Kind { get; private init; }
    }
}
