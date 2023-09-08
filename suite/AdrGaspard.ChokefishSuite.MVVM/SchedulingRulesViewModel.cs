using CommunityToolkit.Mvvm.ComponentModel;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class SchedulingRulesViewModel : ObservableObject
    {
        private ulong _thinkTimeInMs;

        public SchedulingRulesViewModel()
        {
            _thinkTimeInMs = 100;
        }


        public ulong ThinkTimeInMs
        {
            get => _thinkTimeInMs;
            set
            {
                if (value > 0)
                {
                    SetProperty(ref _thinkTimeInMs, value);
                }
            }
        }
    }
}
