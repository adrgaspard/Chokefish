using AdrGaspard.ChokefishSuite.UI.Avalonia.ViewModels;
using Avalonia.Controls;
using Avalonia.Input;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views.Board
{
    public partial class SquareUC : UserControl
    {
        public SquareUC()
        {
            InitializeComponent();
        }

        private void OnPointerPressed(object? sender, PointerPressedEventArgs eventArgs)
        {
            if (DataContext is SquareViewModel squareVM && eventArgs.GetCurrentPoint(this).Properties.IsLeftButtonPressed)
            {
                squareVM.ToggleSelectionCommand.Execute(null);
            }
        }
    }
}
