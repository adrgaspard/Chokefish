using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class RatioViewModel : ObservableObject
    {
        private int _victoryCount;
        private int _drawCount;
        private int _defeatCount;

        public RatioViewModel()
        {
            _victoryCount = 0;
            _drawCount = 0;
            _defeatCount = 0;
            TotalCount = 0;
            ResetCommand = new RelayCommand(Reset);
        }

        public int VictoryCount
        {
            get => _victoryCount;
            set
            {
                if (_victoryCount != value)
                {
                    TotalCount += value - _victoryCount;
                    SetProperty(ref _victoryCount, value);
                    OnRatioChanged();
                }
            }
        }

        public int DrawCount
        {
            get => _drawCount;
            set
            {
                if (_drawCount != value)
                {
                    TotalCount += value - _drawCount;
                    SetProperty(ref _drawCount, value);
                    OnRatioChanged();
                }
            }
        }

        public int DefeatCount
        {
            get => _defeatCount;
            set
            {
                if (_defeatCount != value)
                {
                    TotalCount += value - _defeatCount;
                    SetProperty(ref _defeatCount, value);
                    OnRatioChanged();
                }
            }
        }

        public int TotalCount { get; private set; }

        public double VictoryRatio => TotalCount == 0 ? 0 : VictoryCount / (double)TotalCount;
        public double DrawRatio => TotalCount == 0 ? 0 : DrawCount / (double)TotalCount;
        public double DefeatRatio => TotalCount == 0 ? 0 : DefeatCount / (double)TotalCount;

        public ICommand ResetCommand { get; private init; }

        private void Reset()
        {
            VictoryCount = 0;
            DrawCount = 0;
            DefeatCount = 0;
            OnRatioChanged();
        }

        private void OnRatioChanged()
        {
            OnPropertyChanged(nameof(TotalCount));
            OnPropertyChanged(nameof(VictoryRatio));
            OnPropertyChanged(nameof(DrawRatio));
            OnPropertyChanged(nameof(DefeatRatio));
        }
    }
}
