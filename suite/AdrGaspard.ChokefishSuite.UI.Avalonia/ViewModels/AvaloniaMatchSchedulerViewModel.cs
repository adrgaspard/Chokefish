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
        private bool _boardUpdateScheduled;

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
            Volatile.Write(ref _pendingBoardUpdate, new BoardUpdate { Board = board, Move = move });
            if (_boardUpdateScheduled)
            {
                return;
            }
            _boardUpdateScheduled = true;
            Dispatcher.UIThread.Post(ApplyPendingBoardUpdate);
        }

        private void ScheduleBoardReset()
        {
            Volatile.Write(ref _pendingBoardUpdate, null);
            Dispatcher.UIThread.Post(() => BoardVM.ResetBoardCommand.Execute(null));
        }

        private void ApplyPendingBoardUpdate()
        {
            _boardUpdateScheduled = false;
            BoardUpdate? update = Volatile.Read(ref _pendingBoardUpdate);
            Volatile.Write(ref _pendingBoardUpdate, null);
            if (update is not null)
            {
                BoardVM.SetBoardCommand.Execute((update.Board, update.Move));
            }
        }
    }
}
