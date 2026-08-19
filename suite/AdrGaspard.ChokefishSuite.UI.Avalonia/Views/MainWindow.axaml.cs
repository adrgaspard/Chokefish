using AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels;
using Avalonia;
using Avalonia.Controls;
using Avalonia.VisualTree;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            WindowState = WindowState.Maximized;
            // Les DataContext sont posés dans l'ordre feuilles vers racine pour que
            // les bindings compilés ne voient jamais de DataContext intermédiaire invalide
            ViewModelLocator locator = (ViewModelLocator)Application.Current!.Resources["Locator"]!;
            schedulerGameUC.DataContext = locator.MatchSchedulerVM.BoardVM;
            botVsHumanGameUC.DataContext = locator.BotVsHumanMatchVM.BoardVM;
            schedulerGrid.DataContext = locator.MatchSchedulerVM;
            botVsHumanGrid.DataContext = locator.BotVsHumanMatchVM;
            DataContext = locator.MainVM;
        }
    }
}
