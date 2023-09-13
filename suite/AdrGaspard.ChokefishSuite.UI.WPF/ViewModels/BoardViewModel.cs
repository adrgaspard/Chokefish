using AdrGaspard.ChokefishSuite.Core.GameData;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows.Input;

namespace AdrGaspard.ChokefishSuite.UI.WPF.ViewModels
{
    public class BoardViewModel : ObservableObject
    {
        private static readonly ImmutableArray<ChessPieceType> AllowedPromotionTypes = ImmutableArray.Create(ChessPieceType.Queen, ChessPieceType.Rook, ChessPieceType.Bishop, ChessPieceType.Knight);
        private ChessBoard? _board;
        private readonly List<(ChessBoard, ChessMove?)> _boardStack;
        private int _selectedBoardIndex;
        private ChessColor _perspective;
        private ChessGameResult _gameResult;
        private ChessPieceType _selectedPromotionType;
        private IReadOnlyList<RankViewModel> _ranksFromPerspective;
        private SquareViewModel? _selectedSquareVM;

        public BoardViewModel(bool selectionEventsEnabled)
        {
            _boardStack = new();
            _selectedBoardIndex = -1;
            _perspective = ChessColor.White;
            _gameResult = ChessGameResult.None;
            _selectedPromotionType = PromotionTypes.First();
            SelectionEventsEnabled = selectionEventsEnabled;
            SquareViewModel[] squares = new SquareViewModel[ChessConsts.SquaresCount];
            for (int squareIndex = 0; squareIndex < squares.Length; squareIndex++)
            {
                int fileIndex = squareIndex % ChessConsts.FilesCount;
                int rankIndex = squareIndex / ChessConsts.FilesCount;
                SquareViewModel squareVM = new(fileIndex, rankIndex, (fileIndex + rankIndex) % 2 == 0);
                squares[squareIndex] = squareVM;
                if (SelectionEventsEnabled)
                {
                    squareVM.SelectionToggled += OnSquareViewModelSelectionToggled;
                }
            }
            Squares = squares.ToImmutableArray();
            RankViewModel[] ranks = new RankViewModel[ChessConsts.RanksCount];
            for (int rankIndex = 0; rankIndex < ChessConsts.RanksCount; rankIndex++)
            {
                SquareViewModel[] rankSquares = new SquareViewModel[ChessConsts.FilesCount];
                for (int fileIndex = 0; fileIndex < ChessConsts.FilesCount; fileIndex++)
                {
                    rankSquares[fileIndex] = Squares[(rankIndex * ChessConsts.FilesCount) + fileIndex];
                }
                ranks[rankIndex] = new RankViewModel(rankSquares.ToImmutableArray());
            }
            ImmutableRanks = ranks.ToImmutableArray();
            _ranksFromPerspective = ImmutableRanks;
            _selectedSquareVM = null;

            SetBoardCommand = new RelayCommand<(ChessBoard, ChessMove?)>(SetBoard);
            ResetBoardCommand = new RelayCommand(ResetBoard);
            TogglePerspectiveCommand = new RelayCommand(TogglePerspective);
            DisplayFirstPositionCommand = new RelayCommand(DisplayFirstPosition);
            DisplayPreviousPositionCommand = new RelayCommand(DisplayPreviousPosition);
            DisplayNextPositionCommand = new RelayCommand(DisplayNextPosition);
            DisplayLastPositionCommand = new RelayCommand(DisplayLastPosition);
            RefreshAnnotations();
            RefreshRanksPerspective();
        }

        public bool SelectionEventsEnabled { get; private init; }

        public ImmutableArray<SquareViewModel> Squares { get; private init; }

        public ImmutableArray<RankViewModel> ImmutableRanks { get; private init; }

        public ChessColor Perspective
        {
            get => _perspective;
            private set
            {
                if (_perspective != value && value != ChessColor.None)
                {
                    SetProperty(ref _perspective, value);
                    RefreshAnnotations();
                    RefreshRanksPerspective();
                }
            }
        }

        public ChessGameResult GameResult
        {
            get => _gameResult;
            private set => SetProperty(ref _gameResult, value);
        }

        public IReadOnlyList<ChessPieceType> PromotionTypes => AllowedPromotionTypes;

        public ChessPieceType SelectedPromotionType
        {
            get => _selectedPromotionType;
            set
            {
                if (_selectedPromotionType != value && PromotionTypes.Contains(value))
                {
                    SetProperty(ref _selectedPromotionType, value);
                }
            }
        }

        public IReadOnlyList<RankViewModel> RanksFromPerspective
        {
            get => _ranksFromPerspective;
            private set => SetProperty(ref _ranksFromPerspective, value);
        }

        public ICommand SetBoardCommand { get; private init; }

        public ICommand ResetBoardCommand { get; private init; }

        public ICommand TogglePerspectiveCommand { get; private init; }

        public ICommand DisplayFirstPositionCommand { get; private init; }

        public ICommand DisplayPreviousPositionCommand { get; private init; }

        public ICommand DisplayNextPositionCommand { get; private init; }

        public ICommand DisplayLastPositionCommand { get; private init; }

        public event EventHandler<ChessMove>? MoveChosen;

        private void SetBoard((ChessBoard, ChessMove?) tuple)
        {
            _board = tuple.Item1;
            _selectedBoardIndex = -1;
            _boardStack.Add(tuple);
            UpdateDisplayedBoard();
        }

        private void ResetBoard()
        {
            for (int rankIndex = 0; rankIndex < ChessConsts.RanksCount; rankIndex++)
            {
                for (int fileIndex = 0; fileIndex < ChessConsts.FilesCount; fileIndex++)
                {
                    SquareViewModel squareVM = Squares[(rankIndex * ChessConsts.FilesCount) + fileIndex];
                    squareVM.Piece = ChessPiece.NoPiece;
                    squareVM.CanBeNextMoveDestination = false;
                    squareVM.IsLastMove = false;
                }
            }
            _boardStack.Clear();
            _selectedBoardIndex = -1;
            GameResult = ChessGameResult.None;
            _board = null;
            ResetSquareSelectionAndDestinations();
        }

        private void RefreshAnnotations()
        {
            for (int squareIndex = 0; squareIndex < Squares.Length; squareIndex++)
            {
                int fileIndex = squareIndex % ChessConsts.FilesCount;
                int rankIndex = squareIndex / ChessConsts.FilesCount;
                Squares[squareIndex].HasFileAnnotation = rankIndex == (Perspective == ChessColor.White ? 0 : ChessConsts.RanksCount - 1);
                Squares[squareIndex].HasRankAnnotation = fileIndex == (Perspective == ChessColor.White ? ChessConsts.FilesCount - 1 : 0);
            }
        }

        private void RefreshRanksPerspective()
        {
            RanksFromPerspective = new ReadOnlyCollection<RankViewModel>(Perspective == ChessColor.White ? ImmutableRanks.Reverse().ToList() : ImmutableRanks);
            foreach (RankViewModel rank in RanksFromPerspective)
            {
                rank.RefreshPerspective(Perspective);
            }
        }

        private void OnSquareViewModelSelectionToggled(object? sender, EventArgs eventArgs)
        {
            if (sender is SquareViewModel squareVM && _board is ChessBoard board && _selectedBoardIndex == -1)
            {
                if (_selectedSquareVM is null)
                {
                    SetSquareSelectionAndDestinations(squareVM, board);
                }
                else if (_selectedSquareVM == squareVM)
                {
                    ResetSquareSelectionAndDestinations();
                }
                else
                {
                    ImmutableList<ChessMove> potentialMoves = board.NextMoves.Where(move => move.OldSquare == _selectedSquareVM.Square && move.NewSquare == squareVM.Square).ToImmutableList();
                    ResetSquareSelectionAndDestinations();
                    if (potentialMoves.Count > 0)
                    {
                        ChessMove move = potentialMoves.Count == 1 ? potentialMoves.First() : potentialMoves.First(move => move.PromotionType == SelectedPromotionType);
                        MoveChosen?.Invoke(this, move);
                    }
                    else
                    {
                        SetSquareSelectionAndDestinations(squareVM, board);
                    }
                }
            }
        }

        private void ResetSquareSelectionAndDestinations()
        {
            if (_selectedSquareVM is not null)
            {
                _selectedSquareVM.IsSelected = false;
                _selectedSquareVM = null;
            }
            foreach (SquareViewModel squareIterator in Squares)
            {
                squareIterator.CanBeNextMoveDestination = false;
            }
        }

        private void SetSquareSelectionAndDestinations(SquareViewModel squareVM, ChessBoard board)
        {
            ResetSquareSelectionAndDestinations();
            _selectedSquareVM = squareVM;
            squareVM.IsSelected = true;
            foreach (ChessSquare square in board.NextMoves.Where(move => move.OldSquare == _selectedSquareVM.Square).Select(move => move.NewSquare).Distinct())
            {
                Squares[(square.Rank * ChessConsts.FilesCount) + square.File].CanBeNextMoveDestination = true;
            }
        }

        private void TogglePerspective()
        {
            Perspective = Perspective == ChessColor.White ? ChessColor.Black : ChessColor.White;
        }

        private void DisplayFirstPosition()
        {
            if (_boardStack.Count > 1 && _selectedBoardIndex != 0)
            {
                _selectedBoardIndex = 0;
                UpdateDisplayedBoard();
            }
        }

        private void DisplayPreviousPosition()
        {
            if (_boardStack.Count > 1 && _selectedBoardIndex != 0)
            {
                _selectedBoardIndex = _selectedBoardIndex == -1 ? _boardStack.Count - 2 : _selectedBoardIndex - 1;
                UpdateDisplayedBoard();
            }
        }

        private void DisplayNextPosition()
        {
            if (_boardStack.Count > 1 && _selectedBoardIndex != -1)
            {
                _selectedBoardIndex = _selectedBoardIndex == _boardStack.Count - 2 ? -1 : _selectedBoardIndex + 1;
                UpdateDisplayedBoard();
            }
        }

        private void DisplayLastPosition()
        {
            if (_boardStack.Count > 1 && _selectedBoardIndex != -1)
            {
                _selectedBoardIndex = -1;
                UpdateDisplayedBoard();
            }
        }

        private void UpdateDisplayedBoard()
        {
            (ChessBoard board, ChessMove? move) = _boardStack[_selectedBoardIndex == -1 ? ^1 : _selectedBoardIndex];
            for (int rankIndex = 0; rankIndex < ChessConsts.RanksCount; rankIndex++)
            {
                for (int fileIndex = 0; fileIndex < ChessConsts.FilesCount; fileIndex++)
                {
                    SquareViewModel squareVM = Squares[(rankIndex * ChessConsts.FilesCount) + fileIndex];
                    squareVM.Piece = board[fileIndex, rankIndex];
                    squareVM.IsLastMove = move is not null && (squareVM.Square == move.Value.OldSquare || squareVM.Square == move.Value.NewSquare);
                }
            }
            GameResult = board.Result;
            ResetSquareSelectionAndDestinations();
        }
    }
}
