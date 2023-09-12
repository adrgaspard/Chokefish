using AdrGaspard.ChokefishSuite.UI.WPF.ViewModels;
using System.Windows.Controls;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.UI.WPF.Views.Board
{
    /// <summary>
    /// Logique d'interaction pour SquareUC.xaml
    /// </summary>
    public partial class SquareUC : UserControl
    {
        public SquareUC()
        {
            InitializeComponent();
        }

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs eventArgs)
        {
            base.OnMouseLeftButtonDown(eventArgs);
            if (DataContext is SquareViewModel squareVM)
            {
                squareVM.ToggleSelectionCommand.Execute(null);
            }
        }
    }
}
