using AdrGaspard.ChokefishSuite.Core.GameData;
using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Collections.ObjectModel;
using System.Linq;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class RankViewModel : ObservableObject
    {
        private IReadOnlyList<SquareViewModel> _squaresFromPerspective;

        public RankViewModel(ImmutableArray<SquareViewModel> immutableSquares)
        {
            ImmutableSquares = immutableSquares;
            _squaresFromPerspective = ImmutableSquares;
        }

        public ImmutableArray<SquareViewModel> ImmutableSquares { get; private init; }

        public IReadOnlyList<SquareViewModel> SquaresFromPerspective
        {
            get => _squaresFromPerspective;
            private set => SetProperty(ref _squaresFromPerspective, value);
        }

        public void RefreshPerspective(ChessColor perspective)
        {
            SquaresFromPerspective = new ReadOnlyCollection<SquareViewModel>(perspective == ChessColor.White ? ImmutableSquares : ImmutableSquares.Reverse().ToList());
        }
    }
}
