using AdrGaspard.ChokefishSuite.Core.SearchData;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class SearchInfosViewModel : ObservableObject
    {
        private bool _isValid;
        private int? _depth;
        private TimeSpan? _time;
        private long? _nodes;
        private bool? _isMate;
        private int? _mateScore;
        private double? _centipawnScore;

        public SearchInfosViewModel()
        {
            SetSearchInfosCommand = new RelayCommand<SearchDebugInfos?>(SetSearchInfos);
        }

        public bool IsValid
        {
            get => _isValid;
            private set => SetProperty(ref _isValid, value);
        }

        public int? Depth
        {
            get => _depth;
            private set => SetProperty(ref _depth, value);
        }

        public TimeSpan? Time
        {
            get => _time;
            private set => SetProperty(ref _time, value);
        }

        public long? Nodes
        {
            get => _nodes;
            private set => SetProperty(ref _nodes, value);
        }

        public bool? IsMate
        {
            get => _isMate;
            private set => SetProperty(ref _isMate, value);
        }

        public int? MateScore
        {
            get => _mateScore;
            private set => SetProperty(ref _mateScore, value);
        }

        public double? CentipawnScore
        {
            get => _centipawnScore;
            private set => SetProperty(ref _centipawnScore, value);
        }

        public ICommand SetSearchInfosCommand { get; private init; }

        private void SetSearchInfos(SearchDebugInfos? searchInfos)
        {
            if (searchInfos is SearchDebugInfos value)
            {
                IsValid = true;
                Depth = value.Depth;
                Time = value.Time;
                Nodes = value.Nodes;
                IsMate = value.IsMate;
                MateScore = value.MateScore;
                CentipawnScore = value.CentipawnScore;
            }
            else
            {
                IsValid = false;
                Depth = null;
                Time = null;
                Nodes = null;
                IsMate = null;
                MateScore = null;
                CentipawnScore = null;
            }
        }
    }
}
