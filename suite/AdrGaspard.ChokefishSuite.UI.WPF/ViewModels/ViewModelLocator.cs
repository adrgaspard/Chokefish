using AdrGaspard.ChokefishSuite.Core.UCI;
using AdrGaspard.ChokefishSuite.MVVM;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class ViewModelLocator
    {
        public MainViewModel MainVM { get; init; }

        public MatchMakerViewModel MatchMakerVM { get; init; }

        public ViewModelLocator()
        {
            MainVM = new();
            MatchMakerVM = new(new UciChessEngine(@"C:\Users\Gaspard\Git projects\Chokefish\engine\build"), new UciChessEngine(@"C:\Users\Gaspard\Git projects\Chokefish\engine\build"), UciCommands.PositionArgumentStartpos, 200);
        }
    }
}
