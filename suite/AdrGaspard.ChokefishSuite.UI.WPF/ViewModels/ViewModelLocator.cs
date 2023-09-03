namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class ViewModelLocator
    {
        public MainViewModel MainVM { get; init; }

        public WpfMatchSchedulerViewModel MatchSchedulerVM { get; init; }

        public ViewModelLocator()
        {
            MainVM = new();
            MatchSchedulerVM = new();
        }
    }
}
