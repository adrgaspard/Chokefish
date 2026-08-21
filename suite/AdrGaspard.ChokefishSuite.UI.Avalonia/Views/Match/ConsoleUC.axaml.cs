using AdrGaspard.ChokefishSuite.Core.Utils;
using AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels;
using Avalonia.Controls;
using Avalonia.Controls.Documents;
using Avalonia.Media;
using Avalonia.Threading;
using System.Collections.Specialized;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views.Match
{
    public partial class ConsoleUC : UserControl
    {
        private static readonly IBrush SentBrush = new SolidColorBrush(Color.Parse("#4AD8FF"));
        private static readonly IBrush ReceivedBrush = new SolidColorBrush(Color.Parse("#F0C851"));
        private static readonly IBrush ReceivedInfoBrush = new SolidColorBrush(Color.FromArgb(0x80, 0xF0, 0xC8, 0x51));
        private static readonly IBrush ErrorBrush = new SolidColorBrush(Color.Parse("#F72F2F"));

        private ConsoleViewModel? _consoleVM;

        public ConsoleUC()
        {
            InitializeComponent();
            DataContextChanged += OnDataContextChanged;
        }

        private void OnDataContextChanged(object? sender, EventArgs eventArgs)
        {
            if (_consoleVM is not null)
            {
                _consoleVM.Entries.CollectionChanged -= OnEntriesCollectionChanged;
            }
            _consoleVM = DataContext as ConsoleViewModel;
            consoleText.Inlines?.Clear();
            if (_consoleVM is not null)
            {
                _consoleVM.Entries.CollectionChanged += OnEntriesCollectionChanged;
                foreach (IOTransmitterLog log in _consoleVM.Entries)
                {
                    AddLogLine(log);
                }
            }
            ScrollToEnd();
        }

        private void OnEntriesCollectionChanged(object? sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            void Update()
            {
                if (eventArgs.Action == NotifyCollectionChangedAction.Add && eventArgs.NewItems is not null)
                {
                    foreach (IOTransmitterLog log in eventArgs.NewItems)
                    {
                        AddLogLine(log);
                    }
                }
                else if (eventArgs.Action == NotifyCollectionChangedAction.Remove)
                {
                    int removed = eventArgs.OldItems?.Count ?? 0;
                    for (int i = 0; i < removed; i++)
                    {
                        RemoveFirstLogLine();
                    }
                }
                else
                {
                    consoleText.Inlines?.Clear();
                    if (_consoleVM is not null)
                    {
                        foreach (IOTransmitterLog log in _consoleVM.Entries)
                        {
                            AddLogLine(log);
                        }
                    }
                }
                ScrollToEnd();
            }
            if (Dispatcher.UIThread.CheckAccess())
            {
                Update();
            }
            else
            {
                Dispatcher.UIThread.Post(Update);
            }
        }

        private void AddLogLine(IOTransmitterLog log)
        {
            consoleText.Inlines?.Add(new Run(log.Text)
            {
                Foreground = log.Kind switch
                {
                    IOTransmitterLogKind.Sent => SentBrush,
                    IOTransmitterLogKind.Error => ErrorBrush,
                    IOTransmitterLogKind.ReceivedInfo => ReceivedInfoBrush,
                    _ => ReceivedBrush,
                }
            });
            consoleText.Inlines?.Add(new LineBreak());
        }

        private void RemoveFirstLogLine()
        {
            if (consoleText.Inlines is { } inlines && inlines.Count >= 2)
            {
                inlines.RemoveAt(0);
                inlines.RemoveAt(0);
            }
        }

        private void ScrollToEnd()
        {
            consoleScroll.ScrollToEnd();
        }
    }
}
