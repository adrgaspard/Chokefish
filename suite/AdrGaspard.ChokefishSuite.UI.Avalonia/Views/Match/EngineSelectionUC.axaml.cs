using AdrGaspard.ChokefishSuite.MVVM;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views.Match
{
    public partial class EngineSelectionUC : UserControl
    {
        public EngineSelectionUC()
        {
            InitializeComponent();
        }

        private async void OnExecutableSelectionRequested(object? sender, RoutedEventArgs eventArgs)
        {
            if (DataContext is EngineSelectorViewModel engineSelectorVM)
            {
                IReadOnlyList<IStorageFile> files = await TopLevel.GetTopLevel(this)!.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
                {
                    Title = "Select an executable",
                    AllowMultiple = false
                });
                if (files.Count > 0)
                {
                    engineSelectorVM.EnginePath = files[0].Path.LocalPath.Replace('\\', '/');
                }
            }
        }
    }
}
