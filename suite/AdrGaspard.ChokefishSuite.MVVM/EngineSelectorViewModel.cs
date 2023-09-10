using CommunityToolkit.Mvvm.ComponentModel;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class EngineSelectorViewModel : ObservableObject, IValidatable
    {
        private bool _isValid;
        private string _enginePath;
        private string _engineName;
        private bool _useWsl;

        public EngineSelectorViewModel()
        {
            _enginePath = "";
            _engineName = "";
            _useWsl = false;
            _isValid = false;
        }

        public bool IsValid
        {
            get => _isValid;
            private set => SetProperty(ref _isValid, value);
        }

        public string EnginePath
        {
            get => _enginePath;
            set
            {
                if (value == "" || File.Exists(value))
                {
                    SetProperty(ref _enginePath, value);
                    EngineName = Path.GetFileName(value);
                    IsValid = true;
                }
            }
        }

        public string EngineName
        {
            get => _engineName;
            private set => SetProperty(ref _engineName, value);
        }

        public bool UseWsl
        {
            get => _useWsl;
            set => SetProperty(ref _useWsl, value);
        }
    }
}