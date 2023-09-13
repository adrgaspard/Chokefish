using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.MVVM;
using System.ComponentModel;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class WpfMatchSchedulerViewModel : MatchSchedulerViewModel
    {
        public WpfMatchSchedulerViewModel() : base()
        {
            BoardVM = new(false);
        }

        public BoardViewModel BoardVM { get; private init; }

        public override MatchMakerViewModel? MatchMakerVM
        {
            get => base.MatchMakerVM;
            protected set
            {
                base.MatchMakerVM = value;
                BoardVM.ResetBoardCommand.Execute(null);
            }
        }

        protected override void OnMoveExecuted(ChessMove move)
        {
            base.OnMoveExecuted(move);
            foreach (SquareViewModel squareVM in BoardVM.Squares)
            {
                squareVM.IsLastMove = squareVM.Square == move.OldSquare || squareVM.Square == move.NewSquare;
            }
        }

        protected override void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
            base.OnMatchMakerPropertyChanged(sender, eventArgs);
            switch (eventArgs.PropertyName)
            {
                case nameof(MatchMakerViewModel.Board):
                    if (MatchMakerVM!.Board is not null)
                    {
                        BoardVM.SetBoardCommand.Execute(((ChessBoard)MatchMakerVM.Board, MatchMakerVM.LastMove));
                    }
                    break;
                default: break;
            }
        }
    }
}
