using Avalonia.Controls;

namespace AdrGaspard.ChokefishSuite.UI.Avalonia.Views.Board
{
    public partial class GameUC : UserControl
    {
        public GameUC()
        {
            InitializeComponent();
            boardContainer.LayoutUpdated += (_, _) => ConstrainBoardToSquare();
        }

        private void ConstrainBoardToSquare()
        {
            double barHeight = bottomBar.Bounds.Height;
            double availableHeight = boardContainer.Bounds.Height - (barHeight > 0 ? barHeight : 0);
            double side = Math.Min(boardContainer.Bounds.Width, availableHeight);
            if (side > 0 && (board.Width != side || board.Height != side))
            {
                board.Width = side;
                board.Height = side;
            }
            if (side > 0 && bottomBar.Width != side)
            {
                bottomBar.Width = side;
            }
        }
    }
}
