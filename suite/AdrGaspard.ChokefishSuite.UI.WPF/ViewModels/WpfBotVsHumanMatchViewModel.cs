using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.MVVM;
using System.ComponentModel;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class WpfBotVsHumanMatchViewModel : BotVsHumanMatchViewModel
    {
        public WpfBotVsHumanMatchViewModel()
        {
            BoardVM = new(true);
            BoardVM.MoveChosen += OnBoardMoveChosen;
            PropertyChanged += OnPropertyChanged;
        }

        public BoardViewModel BoardVM { get; private init; }

        protected override void OnMoveExecuted(ChessMove move)
        {
            base.OnMoveExecuted(move);
            foreach (SquareViewModel squareVM in BoardVM.Squares)
            {
                squareVM.IsLastMove = squareVM.Square == move.OldSquare || squareVM.Square == move.NewSquare;
            }
        }

        private void OnBoardMoveChosen(object? sender, ChessMove eventArgs)
        {
            if (UserCanPlay)
            {
                ExecutePlayerMoveCommand.Execute(eventArgs);
            }
        }

        private void OnPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(Board))
            {
                if (Board is null)
                {
                    BoardVM.ResetBoardCommand.Execute(null);
                }
                else
                {
                    BoardVM.SetBoardCommand.Execute(Board);
                }
            }
        }
    }
}
