using AdrGaspard.ChokefishSuite.Core.Helpers;
using AdrGaspard.ChokefishSuite.Core.UCI;
using CommunityToolkit.Mvvm.ComponentModel;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class EngineParametersViewModel : ObservableObject
    {
        private bool _isValid;
        private bool _humanPlaysWhite;
        private string _position;
        private bool _debug;
        private bool _ponder;
        private bool _ownBook;
        private int _threads;
        private ulong _thinkTimeInMs;

        public EngineParametersViewModel()
        {
            _isValid = true;
            _humanPlaysWhite = true;
            _position = UciCommands.PositionArgumentStartpos;
            _debug = true;
            _ponder = false;
            _ownBook = true;
            _threads = 1;
            ThinkTimeInMs = 2000;
        }

        public bool IsValid
        {
            get => _isValid;
            private set => SetProperty(ref _isValid, value);
        }

        public bool HumanPlaysWhite
        {
            get => _humanPlaysWhite;
            set => SetProperty(ref _humanPlaysWhite, value);
        }

        public string Position
        {
            get => _position;
            set
            {
                SetProperty(ref _position, value);
                RefreshIsValid();
            }
        }

        public bool Debug
        {
            get => _debug;
            set => SetProperty(ref _debug, value);
        }

        public bool Ponder
        {
            get => _ponder;
            set => SetProperty(ref _ponder, value);
        }

        public bool OwnBook
        {
            get => _ownBook;
            set => SetProperty(ref _ownBook, value);
        }

        public int Threads
        {
            get => _threads;
            set
            {
                SetProperty(ref _threads, value);
                RefreshIsValid();
            }
        }

        public ulong ThinkTimeInMs
        {
            get => _thinkTimeInMs;
            set
            {
                SetProperty(ref _thinkTimeInMs, value);
                RefreshIsValid();
            }
        }

        private void RefreshIsValid()
        {
            IsValid = Threads > 0 && ThinkTimeInMs > 0 && (Position == UciCommands.PositionArgumentStartpos || Position.IsValidFen());
        }
    }
}
