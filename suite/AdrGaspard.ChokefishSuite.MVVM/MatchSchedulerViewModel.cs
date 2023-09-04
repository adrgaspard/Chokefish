using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using CommunityToolkit.Mvvm.ComponentModel;
using System.ComponentModel;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class MatchSchedulerViewModel : ObservableObject
    {
        private MatchMakerViewModel _matchMakerVM;

        public MatchSchedulerViewModel()
        {
            IChessEngine whiteEngine = new UciChessEngine("wsl", @"/mnt/c/Users/Gaspard/Desktop/White/Chokefish", "\n");
            IChessEngine blackEngine = new UciChessEngine("wsl", @"/mnt/c/Users/Gaspard/Desktop/Black/Chokefish", "\n");
            whiteEngine.Initialize();
            blackEngine.Initialize();
            _ = whiteEngine.SetOption(OptionHelper.PonderOptionName, false);
            _ = blackEngine.SetOption(OptionHelper.PonderOptionName, false);
            _matchMakerVM = new(whiteEngine, blackEngine, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 200);
            SubsbribeToMatchMakerPropertyChanged(_matchMakerVM);
        }

        public MatchMakerViewModel MatchMakerVM
        {
            get => _matchMakerVM;
            private set
            {
                if (_matchMakerVM != value)
                {
                    UnsubsbribeFromMatchMakerPropertyChanged(_matchMakerVM);
                    _ = SetProperty(ref _matchMakerVM, value);
                    SubsbribeToMatchMakerPropertyChanged(_matchMakerVM);
                }
            }
        }

        private void SubsbribeToMatchMakerPropertyChanged(MatchMakerViewModel matchMaker)
        {
            matchMaker.PropertyChanged += OnMatchMakerPropertyChanged;
        }

        private void UnsubsbribeFromMatchMakerPropertyChanged(MatchMakerViewModel matchMaker)
        {
            matchMaker.PropertyChanged -= OnMatchMakerPropertyChanged;
        }

        protected virtual void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
        }
    }
}
