namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class ViewModelLocator
    {
        public MainViewModel MainVM { get; private init; }

        public WpfMatchSchedulerViewModel MatchSchedulerVM { get; private init; }

        public WpfBotVsHumanMatchViewModel BotVsHumanMatchVM { get; private init; }

        public ViewModelLocator()
        {
            MainVM = new();
            MatchSchedulerVM = new();
            BotVsHumanMatchVM = new();
        }
    }
}
