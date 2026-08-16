using AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels;
using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia
{
    public partial class App : Application
    {
        public override void Initialize()
        {
            AvaloniaXamlLoader.Load(this);
        }

        public override void OnFrameworkInitializationCompleted()
        {
            if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
            {
                desktop.MainWindow = new Views.MainWindow();
                desktop.Exit += (_, _) => ((ViewModelLocator)Resources["Locator"]!).MatchSchedulerVM.StopMatchCommand.Execute(null);
            }
            base.OnFrameworkInitializationCompleted();
        }
    }
}
