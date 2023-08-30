using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class MatchMakerViewModel : ObservableObject
    {
        private readonly object _lock;
        private readonly IChessEngine _first;
        private readonly IChessEngine _second;
        private ChessBoard? _board;
        private bool _running;
        private CancellationTokenSource _cancellationTokenSource;

        public MatchMakerViewModel(IChessEngine first, IChessEngine second)
        {
            _lock = new();
            _first = first;
            _second = second;
            _cancellationTokenSource = new();
            StartMatchCommand = new RelayCommand(StartMatch);
        }

        public ChessBoard? Board
        {
            get => _board;
            private set => SetProperty(ref _board, value);
        }

        public ICommand StartMatchCommand { get; private init; }

        private void StartMatch()
        {
            bool startMatch = false;
            CancellationToken token;
            lock (_lock)
            {
                if (!_running)
                {
                    _running = true;
                    if (!_cancellationTokenSource.IsCancellationRequested)
                    {
                        _cancellationTokenSource.Cancel();
                    }
                    _cancellationTokenSource = new();
                    token = _cancellationTokenSource.Token;
                    startMatch = true;
                }
            }
            if (startMatch)
            {
                Task.Run(() =>
                {
                    // TODO
                });
            }
        }

        private void StopMatch()
        {
            lock (_lock)
            {
                _cancellationTokenSource.Cancel();
            }
        }
    }
}
