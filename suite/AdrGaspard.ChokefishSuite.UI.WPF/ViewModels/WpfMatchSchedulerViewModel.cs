using AdrGaspard.ChokefishSuite.MVVM;
using System.ComponentModel;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class WpfMatchSchedulerViewModel : MatchSchedulerViewModel
    {
        public WpfMatchSchedulerViewModel() : base()
        {
            BoardVM = new();
        }

        public BoardViewModel BoardVM { get; private init; }

        protected override void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
            base.OnMatchMakerPropertyChanged(sender, eventArgs);
            switch (eventArgs.PropertyName)
            {
                case nameof(MatchMakerViewModel.Board):
                    BoardVM.SetBoardCommand.Execute(MatchMakerVM.Board);
                    break;
                //case nameof(MatchMakerViewModel.Result):
                //    break;
                //case nameof(MatchMakerViewModel.BlackSearchDebugInfos):
                //    break;
                default: break;
            }
        }
    }
}
