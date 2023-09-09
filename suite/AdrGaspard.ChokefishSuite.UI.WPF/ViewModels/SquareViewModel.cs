using AdrGaspard.ChokefishSuite.Core.GameData;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class SquareViewModel : ObservableObject
    {
        private ChessPiece _piece;
        private bool _hasFileAnnotation;
        private bool _hasRankAnnotation;
        private bool _isLastMove;
        private bool _isSelected;
        private bool _canBeNextMoveDestination;

        public SquareViewModel(int fileIndex, int rankIndex, bool isBlackSquare)
        {
            _piece = ChessPiece.NoPiece;
            FileIndex = fileIndex;
            RankIndex = rankIndex;
            Square = new(RankIndex, FileIndex);
            IsBlackSquare = isBlackSquare;
            _hasFileAnnotation = false;
            _hasRankAnnotation = false;
            ToggleSelectionCommand = new RelayCommand(ToggleSelection);
        }

        public int FileIndex { get; private init; }

        public int RankIndex { get; private init; }

        public ChessSquare Square { get; private init; }

        public bool IsBlackSquare { get; private init; }

        public ChessPiece Piece
        {
            get => _piece;
            set => SetProperty(ref _piece, value);
        }

        public bool HasFileAnnotation
        {
            get => _hasFileAnnotation;
            set => SetProperty(ref _hasFileAnnotation, value);
        }

        public bool HasRankAnnotation
        {
            get => _hasRankAnnotation;
            set => SetProperty(ref _hasRankAnnotation, value);
        }

        public bool IsLastMove
        {
            get => _isLastMove;
            set => SetProperty(ref _isLastMove, value);
        }

        public bool IsSelected
        {
            get => _isSelected;
            set => SetProperty(ref _isSelected, value);
        }

        public bool CanBeNextMoveDestination
        {
            get => _canBeNextMoveDestination;
            set => SetProperty(ref _canBeNextMoveDestination, value);
        }

        public ICommand ToggleSelectionCommand { get; private init; }

        public event EventHandler? SelectionToggled;

        private void ToggleSelection()
        {
            SelectionToggled?.Invoke(this, EventArgs.Empty);
        }
    }
}
