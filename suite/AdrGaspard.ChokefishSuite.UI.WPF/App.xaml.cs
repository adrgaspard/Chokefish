using AdrGaspard.ChokefishSuite.UI.WPF.ViewModels;
using System.Windows;

namespace AdrGaspard.ChokefishSuite.UI.WPF
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnExit(ExitEventArgs eventArgs)
        {
            base.OnExit(eventArgs);
            ViewModelLocator locator = (ViewModelLocator)Resources["Locator"];
            locator.MatchSchedulerVM.StopMatchScheduleCommand.Execute(null);
        }
    }
}
