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
            double side = Math.Min(boardContainer.Bounds.Width, boardContainer.Bounds.Height);
            if (side > 0 && (board.Width != side || board.Height != side))
            {
                board.Width = side;
                board.Height = side;
            }
        }
    }
}
