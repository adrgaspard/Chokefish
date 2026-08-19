using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.MVVM;
using System.ComponentModel;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels
{
    public class AvaloniaBotVsHumanMatchViewModel : BotVsHumanMatchViewModel
    {
        public AvaloniaBotVsHumanMatchViewModel()
        {
            BoardVM = new(true);
            ConsoleVM = new();
            BoardVM.MoveChosen += OnBoardMoveChosen;
            PropertyChanged += OnPropertyChanged;
        }

        public BoardViewModel BoardVM { get; private init; }

        public ConsoleViewModel ConsoleVM { get; private init; }

        protected override void OnReset()
        {
            base.OnReset();
            ConsoleVM.Clear();
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
            switch (eventArgs.PropertyName)
            {
                case nameof(Board):
                    if (Board is null)
                    {
                        BoardVM.ResetBoardCommand.Execute(null);
                    }
                    else
                    {
                        BoardVM.SetBoardCommand.Execute(((ChessBoard)Board, LastMove));
                    }
                    break;
                case nameof(BlackName):
                    if ((BlackName is null && BoardVM.Perspective == ChessColor.Black) || (BlackName == HumanPlayerName && BoardVM.Perspective == ChessColor.White))
                    {
                        BoardVM.TogglePerspectiveCommand.Execute(null);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
