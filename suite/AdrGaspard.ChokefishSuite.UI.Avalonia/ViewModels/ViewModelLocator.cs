namespace AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels
{
    public class ViewModelLocator
    {
        public MainViewModel MainVM { get; private init; }

        public AvaloniaMatchSchedulerViewModel MatchSchedulerVM { get; private init; }

        public AvaloniaBotVsHumanMatchViewModel BotVsHumanMatchVM { get; private init; }

        public ViewModelLocator()
        {
            MainVM = new();
            MatchSchedulerVM = new();
            BotVsHumanMatchVM = new();
        }
    }
}
