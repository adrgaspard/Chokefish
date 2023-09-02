namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class ViewModelLocator
    {
        public MainViewModel MainVM { get; init; }

        public BoardViewModel BoardVM { get; init; }

        public ViewModelLocator()
        {
            MainVM = new();
            BoardVM = new();
        }
    }
}
