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
