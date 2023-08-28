using AdrGaspard.ChokefishSuite.Core.Utils;

namespace AdrGaspard.ChokefishSuite.Core.GameData
{
    public readonly struct ChessBoard
    {
        private readonly ChessPiece[,] _squares;
        private readonly ChessCastlingState _whiteCastling;
        private readonly ChessCastlingState _blackCastling;

        public ChessBoard(ChessPiece[,] pieces, ChessColor colorToMove, ChessCastlingState whiteCastling, ChessCastlingState blackCastling, ChessSquare? enPassantSquare, uint silentMoveCount, uint moveCount)
        {
            if (pieces == null || pieces.Rank != 2 || pieces.Length != ChessConsts.SquaresCount)
            {
                throw new ChessException($"The {nameof(pieces)} array must be a 8x8 2D array!");
            }
            _squares = new ChessPiece[ChessConsts.FilesCount, ChessConsts.RanksCount];
            pieces.CopyTo(_squares, 0);
            ColorToMove = colorToMove;
            _whiteCastling = whiteCastling;
            _blackCastling = blackCastling;
            EnPassantSquare = enPassantSquare;
            SilentMoveCount = silentMoveCount;
            MoveCount = moveCount;
        }

        public ChessPiece this[ChessSquare square] => this[square.File, square.Rank];

        public ChessPiece this[int file, int rank] => _squares[file, rank];

        public ChessCastlingState this[ChessColor color] => color switch
        {
            ChessColor.White => _whiteCastling,
            ChessColor.Black => _blackCastling,
            _ => throw new ChessException($"There is no {nameof(ChessCastlingState)} for {color}!")
        };

        public readonly ChessColor ColorToMove { get; private init; }

        public readonly ChessSquare? EnPassantSquare { get; private init; }

        public readonly uint SilentMoveCount { get; private init; }

        public readonly uint MoveCount { get; private init; }
    }
}
