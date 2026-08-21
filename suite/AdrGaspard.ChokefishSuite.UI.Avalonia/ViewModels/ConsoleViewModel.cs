using AdrGaspard.ChokefishSuite.Core.Utils;
using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.ObjectModel;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels
{
    public class ConsoleViewModel : ObservableObject
    {
        private const int MaxEntries = 10000;
        private const int TrimChunk = 1000;
        public ConsoleViewModel()
        {
            Entries = new();
            IOTransmitter.LogEmitted += OnLogEmitted;
        }

        public ObservableCollection<IOTransmitterLog> Entries { get; private init; }

        public void Clear()
        {
            Entries.Clear();
        }

        private void OnLogEmitted(object? sender, IOTransmitterLog log)
        {
            Entries.Add(log);
            if (Entries.Count > MaxEntries)
            {
                int excess = Entries.Count - MaxEntries + TrimChunk;
                for (int i = 0; i < excess; i++)
                {
                    Entries.RemoveAt(0);
                }
            }
        }
    }
}
