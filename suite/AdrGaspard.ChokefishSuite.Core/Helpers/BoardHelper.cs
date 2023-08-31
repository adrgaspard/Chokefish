using AdrGaspard.ChokefishSuite.Core.GameData;
using AdrGaspard.ChokefishSuite.Core.UCI;
using AdrGaspard.ChokefishSuite.Core.Utils;
namespace AdrGaspard.ChokefishSuite.Core.Helpers
{
    public static class BoardHelper
    {
        public const string StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        public static ChessBoard? ToChessBoard(this string str)
        {
            string[] splitedArguments = str.Split($" {UciResponses.FenArgumentResult} ");
            ChessGameResult result;
            if (splitedArguments.Length == 2)
            {
                switch (splitedArguments[1])
                {
                    case UciResponses.FenArgumentResultArgumentPlaying:
                        result = ChessGameResult.Playing;
                        break;
                    case UciResponses.FenArgumentResultArgumentWhiteMated:
                        result = ChessGameResult.WhiteMated;
                        break;
                    case UciResponses.FenArgumentResultArgumentBlackMated:
                        result = ChessGameResult.BlackMated;
                        break;
                    case UciResponses.FenArgumentResultArgumentStalemate:
                        result = ChessGameResult.Stalemate;
                        break;
                    case UciResponses.FenArgumentResultArgumentRepetition:
                        result = ChessGameResult.Repetition;
                        break;
                    case UciResponses.FenArgumentResultArgumentFiftyMoveRule:
                        result = ChessGameResult.FiftyMoveRule;
                        break;
                    case UciResponses.FenArgumentResultArgumentInsufficientMaterial:
                        result = ChessGameResult.InsufficientMaterial;
                        break;
                    default: return null;
                }
                return splitedArguments[0].ToChessBoard(result);
            }
            return null;
        }

        private static ChessBoard? ToChessBoard(this string fenString, ChessGameResult result)
        {
            try
            {
                string[] informations = fenString.Split(' ');
                if (informations?.Length != 6)
                {
                    throw new ChessException($"The data '{fenString}' is incorrect.");
                }
                ChessPiece[,] squares = GenerateSquares(informations[0]);
                ChessColor currentPlayer = GenerateCurrentPlayer(informations[1]);
                (ChessCastlingState, ChessCastlingState) castlingLegality = GenerateCastlingLegality(informations[2]);
                ChessSquare? enPassantTarget = GenerateEnPassantTarget(informations[3]);
                byte fiftyMoveCounter = (byte)GenerateFiftyMoveCounter(informations[4]);
                uint totalMoveCounter = GenerateTotalMoveCounter(informations[5]);
                return new(squares, currentPlayer, castlingLegality.Item1, castlingLegality.Item2, enPassantTarget, fiftyMoveCounter, totalMoveCounter, result);
            }
            catch (Exception) { }
            return null;
        }

        private static ChessPiece[,] GenerateSquares(string configPart)
        {
            int file, rank;
            ChessPiece[,] squares = new ChessPiece[ChessConsts.FilesCount, ChessConsts.RanksCount];
            for (file = 0; file < ChessConsts.FilesCount; file++)
            {
                for (rank = 0; rank < ChessConsts.RanksCount; rank++)
                {
                    squares[file, rank] = ChessPiece.NoPiece;
                }
            }
            file = 0;
            rank = ChessConsts.RanksCount - 1;
            try
            {
                foreach (char symbol in configPart)
                {
                    if (symbol == '/')
                    {
                        file = 0;
                        rank--;
                    }
                    else
                    {
                        if (char.IsDigit(symbol))
                        {
                            file += (int)char.GetNumericValue(symbol);
                        }
                        else
                        {
                            squares[file, rank] = new(PieceTypeHelper.SymbolsToPieceType[symbol.ToString().ToLower()[0]], char.IsUpper(symbol) ? ChessColor.White : ChessColor.Black);
                            file++;
                        }
                    }
                }
            }
            catch (Exception)
            {
                throw new ChessException($"The configuration part '{configPart}' is incorrect.");
            }
            return squares;
        }

        private static ChessColor GenerateCurrentPlayer(string configPart)
        {
            return configPart == "w" ? ChessColor.White : configPart == "b" ? ChessColor.Black : throw new ArgumentException($"The configuration part '{configPart}' is incorrect.");
        }

        private static (ChessCastlingState, ChessCastlingState) GenerateCastlingLegality(string configPart)
        {
            ChessCastlingState whiteCastling = ChessCastlingState.None;
            ChessCastlingState blackCastling = ChessCastlingState.None;
            if (configPart != "-")
            {
                foreach (char symbol in configPart)
                {
                    switch (symbol)
                    {
                        case 'K': whiteCastling |= ChessCastlingState.KingSide; break;
                        case 'k': blackCastling |= ChessCastlingState.KingSide; break;
                        case 'Q': whiteCastling |= ChessCastlingState.QueenSide; break;
                        case 'q': blackCastling |= ChessCastlingState.QueenSide; break;
                        default: throw new ChessException($"The configuration part '{configPart}' is incorrect.");
                    }
                }
            }
            return (whiteCastling, blackCastling);
        }

        private static ChessSquare? GenerateEnPassantTarget(string configPart)
        {
            return configPart == "-"
                ? null
                : configPart.Length == 2
                ? configPart.ToChessSquare()
                : throw new ChessException($"The configuration part '{configPart}' is incorrect.");
        }

        private static uint GenerateFiftyMoveCounter(string configPart)
        {
            return ParseStringToUintOrThrow(configPart);
        }

        private static uint GenerateTotalMoveCounter(string configPart)
        {
            return ParseStringToUintOrThrow(configPart);
        }

        private static uint ParseStringToUintOrThrow(string str)
        {
            return uint.TryParse(str, out uint result) ? result : throw new ChessException($"The configuration part '{str}' is incorrect.");
        }
    }
}
