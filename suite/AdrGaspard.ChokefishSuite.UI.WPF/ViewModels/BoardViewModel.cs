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
        private ChessColor _perspective;
        private ChessGameResult _gameResult;
        private uint _moveCount;
        private IReadOnlyList<RankViewModel> _ranksFromPerspective;

        public BoardViewModel(bool selectionEventsEnabled)
        {
            _perspective = ChessColor.White;
            _gameResult = ChessGameResult.None;
            _moveCount = 0;
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
            SetBoardCommand = new RelayCommand<ChessBoard>(SetBoard);
            ResetBoardCommand = new RelayCommand(ResetBoard);
            RefreshAnnotations();
            RefreshRanksPerspective();
        }

        public bool SelectionEventsEnabled { get; private init; }

        public ImmutableArray<SquareViewModel> Squares { get; private init; }

        public ImmutableArray<RankViewModel> ImmutableRanks { get; private init; }

        public ChessColor Perspective
        {
            get => _perspective;
            set
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

        public uint MoveCount
        {
            get => _moveCount;
            private set => SetProperty(ref _moveCount, value);
        }

        public IReadOnlyList<RankViewModel> RanksFromPerspective
        {
            get => _ranksFromPerspective;
            private set => SetProperty(ref _ranksFromPerspective, value);
        }

        public ICommand SetBoardCommand { get; private init; }

        public ICommand ResetBoardCommand { get; private init; }

        private void SetBoard(ChessBoard board)
        {
            for (int rankIndex = 0; rankIndex < ChessConsts.RanksCount; rankIndex++)
            {
                for (int fileIndex = 0; fileIndex < ChessConsts.FilesCount; fileIndex++)
                {
                    Squares[(rankIndex * ChessConsts.FilesCount) + fileIndex].Piece = board[fileIndex, rankIndex];
                    GameResult = board.Result;
                    MoveCount = board.MoveCount;
                }
            }
        }

        private void ResetBoard()
        {
            for (int rankIndex = 0; rankIndex < ChessConsts.RanksCount; rankIndex++)
            {
                for (int fileIndex = 0; fileIndex < ChessConsts.FilesCount; fileIndex++)
                {
                    Squares[(rankIndex * ChessConsts.FilesCount) + fileIndex].Piece = ChessPiece.NoPiece;
                    GameResult = ChessGameResult.None;
                    MoveCount = 0;
                }
            }
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
            if (sender is SquareViewModel squareVM)
            {

            }
        }
    }
}
