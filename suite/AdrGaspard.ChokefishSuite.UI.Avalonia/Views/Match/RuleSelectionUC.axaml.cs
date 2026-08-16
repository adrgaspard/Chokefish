using AdrGaspard.ChokefishSuite.MVVM;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views.Match
{
    public partial class RuleSelectionUC : UserControl
    {
        public RuleSelectionUC()
        {
            InitializeComponent();
        }

        private async void OnJsonSelectionRequested(object? sender, RoutedEventArgs eventArgs)
        {
            if (DataContext is SchedulingRulesViewModel schedulingRulesVM)
            {
                IReadOnlyList<IStorageFile> files = await TopLevel.GetTopLevel(this)!.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
                {
                    Title = "Select a json file",
                    AllowMultiple = false,
                    FileTypeFilter = new[] { new FilePickerFileType("Json files") { Patterns = new[] { "*.json" } } }
                });
                if (files.Count > 0)
                {
                    schedulingRulesVM.SetPositionsFromJsonCommand.Execute(files[0].Path.LocalPath.Replace('\\', '/'));
                }
            }
        }
    }
}
