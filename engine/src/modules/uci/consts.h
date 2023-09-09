#ifndef UCI_CONSTS_H
#define UCI_CONSTS_H

// Mutable consts

#define ENGINE_NAME "Chokefish"
#define ENGINE_AUTHOR "Gaspard Andrieu"

// Partially mutable consts

#define MAX_WORKER_THREADS 8
#define COMMAND_STR_LEN 80000
#define CANCELLATION_CHECK_TIME_IN_MS 10000
#define PONDER_FINISHED_CHECK_TIME_IN_MS 100

// Immutable consts

// GE means GUI to Engine, EG means engine to GUI

#define UCI_DELIMITER " "
#define UCI_DELIMITER_CHAR ' '

#define GE_CMD_UCI "uci"

#define GE_CMD_DEBUG "debug"
#define GE_CMD_DEBUG_OPT_ON "on"
#define GE_CMD_DEBUG_OPT_OFF "off"

#define GE_CMD_ISREADY "isready"

#define GE_CMD_SETOPTION "setoption"
#define GE_CMD_SETOPTION_OPT_NAME "name"
#define GE_CMD_SETOPTION_OPT_VALUE "value"

#define GE_CMD_UCINEWGAME "ucinewgame"

#define GE_CMD_POSITION "position"
#define GE_CMD_POSITION_OPT_FEN "fen"
#define GE_CMD_POSITION_OPT_STARTPOS "startpos"
#define GE_CMD_POSITION_OPT_MOVES "moves"

#define GE_CMD_GO "go"
#define GE_CMD_GO_OPT_PONDER "ponder"
#define GE_CMD_GO_OPT_WTIME "wtime"
#define GE_CMD_GO_OPT_BTIME "btime"
#define GE_CMD_GO_OPT_WINC "winc"
#define GE_CMD_GO_OPT_BINC "binc"
#define GE_CMD_GO_OPT_MOVESTOGO "movestogo"
#define GE_CMD_GO_OPT_MOVETIME "movetime"
#define GE_CMD_GO_OPT_INFINITE "infinite"

#define GE_CMD_STOP "stop"

#define GE_CMD_PONDERHIT "ponderhit"

#define GE_CMD_QUIT "quit"

#define GE_CMD_DISPLAY "display"
#define GE_CMD_DISPLAY_OPT_FEN "fen"

#define EG_CMD_ID "id"
#define EG_CMD_ID_OPT_NAME "name"
#define EG_CMD_ID_OPT_AUTHOR "author"

#define EG_CMD_UCIOK "uciok"
#define EG_CMD_READYOK "readyok"

#define EG_CMD_BESTMOVE "bestmove"
#define EG_CMD_BESTMOVE_OPT_PONDER "ponder"

#define EG_CMD_INFO "info"
#define EG_CMD_INFO_OPT_DEPTH "depth"
#define EG_CMD_INFO_OPT_TIME "time"
#define EG_CMD_INFO_OPT_NODES "nodes"
#define EG_CMD_INFO_OPT_SCORE "score"
#define EG_CMD_INFO_OPT_SCORE_OPT_CP "cp"
#define EG_CMD_INFO_OPT_SCORE_OPT_MATE "mate"

#define EG_CMD_OPTION "option"
#define EG_CMD_OPTION_OPT_NAME "name"
#define EG_CMD_OPTION_OPT_TYPE "type"
#define EG_CMD_OPTION_OPT_DEFAULT "default"
#define EG_CMD_OPTION_OPT_MIN "min"
#define EG_CMD_OPTION_OPT_MAX "max"

#define EG_CMD_FEN "fen"
#define EG_CMD_FEN_OPT_RESULT "result"
#define EG_CMD_FEN_OPT_RESULT_OPT_PLAYING "playing"
#define EG_CMD_FEN_OPT_RESULT_OPT_WHITE_MATED "whitemated"
#define EG_CMD_FEN_OPT_RESULT_OPT_BLACK_MATED "blackmated"
#define EG_CMD_FEN_OPT_RESULT_OPT_STALEMATE "stalemate"
#define EG_CMD_FEN_OPT_RESULT_OPT_REPETITION "repetition"
#define EG_CMD_FEN_OPT_RESULT_OPT_FIFTY_MOVE_RULE "fiftymoverule"
#define EG_CMD_FEN_OPT_RESULT_OPT_INSUFFICIENT_MATERIAL "insufficientmaterial"
#define EG_CMD_FEN_OPT_RESULT_OPT_UNKNOWN "unknown"
#define EG_CMD_FEN_OPT_MOVES "moves"

#endif // UCI_CONSTS_H
