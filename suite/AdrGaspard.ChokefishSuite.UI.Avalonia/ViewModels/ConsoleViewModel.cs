using AdrGaspard.ChokefishSuite.Core.Utils;
using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.ObjectModel;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels
{
    public class ConsoleViewModel : ObservableObject
    {
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
        }
    }
}
