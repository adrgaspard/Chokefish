using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.MVVM;
using Avalonia.Threading;
using System.ComponentModel;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels
{
    public class AvaloniaMatchSchedulerViewModel : MatchSchedulerViewModel
    {
        private sealed class BoardUpdate
        {
            public required ChessBoard Board { get; init; }

            public ChessMove? Move { get; init; }
        }

        private BoardUpdate? _pendingBoardUpdate;

        public AvaloniaMatchSchedulerViewModel() : base()
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
                ScheduleBoardReset();
            }
        }

        protected override void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
            base.OnMatchMakerPropertyChanged(sender, eventArgs);
            switch (eventArgs.PropertyName)
            {
                case nameof(MatchMakerViewModel.Board):
                    if (MatchMakerVM!.Board is ChessBoard board)
                    {
                        ScheduleBoardUpdate(board, MatchMakerVM.LastMove);
                    }
                    break;
                default: break;
            }
        }

        private void ScheduleBoardUpdate(ChessBoard board, ChessMove? move)
        {
            BoardUpdate? previous = Interlocked.Exchange(ref _pendingBoardUpdate, new BoardUpdate { Board = board, Move = move });
            if (previous is null)
            {
                Dispatcher.UIThread.Post(ApplyPendingBoardUpdate);
            }
        }

        private void ScheduleBoardReset()
        {
            Interlocked.Exchange(ref _pendingBoardUpdate, null);
            Dispatcher.UIThread.Post(() => BoardVM.ResetBoardCommand.Execute(null));
        }

        private void ApplyPendingBoardUpdate()
        {
            BoardUpdate? update = Interlocked.Exchange(ref _pendingBoardUpdate, null);
            if (update is not null)
            {
                BoardVM.SetBoardCommand.Execute((update.Board, update.Move));
            }
        }
    }
}
