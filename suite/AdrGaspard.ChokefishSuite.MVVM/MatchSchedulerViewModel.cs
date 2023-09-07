using AdrGaspard.ChokefishSuite.Core.Contracts;
using AdrGaspard.ChokefishSuite.Core.GameData;
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
        private readonly object _lock;
        private readonly IChessEngine _firstEngine;
        private readonly IChessEngine _secondEngine;
        private RatioViewModel _ratioVM;
        private MatchMakerViewModel _matchMakerVM;
        private bool _running;
        private CancellationTokenSource _cancellationTokenSource;
        private TaskCompletionSource<bool> _matchCompletionSource;

        public MatchSchedulerViewModel()
        {
            _lock = new();
            _firstEngine = new UciChessEngine("wsl", @"/mnt/c/Users/Gaspard/Desktop/White/Chokefish", "\n");
            _secondEngine = new UciChessEngine("wsl", @"/mnt/c/Users/Gaspard/Desktop/Black/Chokefish", "\n");
            _firstEngine.Initialize();
            _secondEngine.Initialize();
            _ = _firstEngine.SetOption(OptionHelper.PonderOptionName, false);
            _ = _secondEngine.SetOption(OptionHelper.PonderOptionName, false);
            _ratioVM = new();
            _matchMakerVM = new(_firstEngine, _secondEngine, UciCommands.PositionArgumentStartpos, 200);
            _running = false;
            _cancellationTokenSource = new();
            _matchCompletionSource = new();
            SubsbribeToMatchMakerPropertyChanged(_matchMakerVM);
            StartMatchScheduleCommand = new RelayCommand(StartMatchSchedule);
            StopMatchScheduleCommand = new RelayCommand(StopMatchSchedule);
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

        public RatioViewModel RatioVM
        {
            get => _ratioVM;
            private set => SetProperty(ref _ratioVM, value);
        }

        public ICommand StartMatchScheduleCommand { get; private init; }

        public ICommand StopMatchScheduleCommand { get; private init; }

        private void StartMatchSchedule()
        {
            bool startMatch = false;
            CancellationToken token = CancellationToken.None;
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
                    RatioVM.ResetCommand.Execute(null);
                }
            }
            if (startMatch)
            {
                _ = Task.Run(() =>
                {
                    int i = 0;
                    while (!token.IsCancellationRequested)
                    {
                        ResetMatchMaker();
                        MatchMakerVM.MatchCompleted += OnMatchCompletedOrCancelled;
                        MatchMakerVM.MatchCanceled += OnMatchCompletedOrCancelled;
                        _matchCompletionSource = new();
                        MatchMakerVM.StartMatchCommand.Execute(null);
                        _matchCompletionSource.Task.Wait();
                        UpdateRatio(MatchMakerVM.Result);
                        MatchMakerVM.MatchCanceled -= OnMatchCompletedOrCancelled;
                        MatchMakerVM.MatchCompleted -= OnMatchCompletedOrCancelled;
                        if (i++ > 1000)
                        {
                            break;
                        }
                    }
                    lock (_lock)
                    {
                        if (!token.IsCancellationRequested)
                        {
                            _running = false;
                        }
                    }
                });
            }
        }

        private void StopMatchSchedule()
        {
            lock (_lock)
            {
                if (_running)
                {
                    _running = false;
                    MatchMakerVM.StopMatchCommand.Execute(null);
                    _cancellationTokenSource.Cancel();
                }
            }
        }

        protected virtual void ResetMatchMaker()
        {
            MatchMakerVM.StopMatchCommand.Execute(null);
            MatchMakerVM = new(_firstEngine, _secondEngine, UciCommands.PositionArgumentStartpos, 200);
        }

        private void UpdateRatio(ChessGameResult result)
        {
            switch (result)
            {
                case ChessGameResult.WhiteMated:
                    RatioVM.DefeatCount++;
                    break;
                case ChessGameResult.BlackMated:
                    RatioVM.VictoryCount++;
                    break;
                case ChessGameResult.Stalemate:
                case ChessGameResult.Repetition:
                case ChessGameResult.FiftyMoveRule:
                case ChessGameResult.InsufficientMaterial:
                    RatioVM.DrawCount++;
                    break;
                case ChessGameResult.Playing:
                case ChessGameResult.None:
                default:
                    break;
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

        private void OnMatchCompletedOrCancelled(object? sender, EventArgs eventArgs)
        {
            _matchCompletionSource.SetResult(true);
        }

        protected virtual void OnMatchMakerPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
        {
        }
    }
}
