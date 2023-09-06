using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.ComponentModel;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class MatchSchedulerViewModel : ObservableObject
    {
        private readonly IChessEngine _whiteEngine;
        private readonly IChessEngine _blackEngine;
        private MatchMakerViewModel _matchMakerVM;

        public MatchSchedulerViewModel()
        {
            _whiteEngine = new UciChessEngine("wsl", @"/mnt/c/Users/Gaspard/Desktop/White/Chokefish", "\n");
            _blackEngine = new UciChessEngine("wsl", @"/mnt/c/Users/Gaspard/Desktop/Black/Chokefish", "\n");
            _whiteEngine.Initialize();
            _blackEngine.Initialize();
            _ = _whiteEngine.SetOption(OptionHelper.PonderOptionName, false);
            _ = _blackEngine.SetOption(OptionHelper.PonderOptionName, false);
            _matchMakerVM = new(_whiteEngine, _blackEngine, "startpos", 200);
            ResetMatchMakerCommand = new RelayCommand(ResetMatchMaker);
            SubsbribeToMatchMakerPropertyChanged(_matchMakerVM);
        }

        public ICommand ResetMatchMakerCommand { get; private init; }

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

        protected virtual void ResetMatchMaker()
        {
            MatchMakerVM.StopMatchCommand.Execute(null);
            MatchMakerVM = new(_whiteEngine, _blackEngine, "startpos", 200);
        }

        protected virtual void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
        }
    }
}
