#include <string.h>
#include "../core/logging.h"
#include "../core/move_generator.h"
#include "../core/piece.h"
#include "../core/position.h"
#include "../game_tools/game_result.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "commands.h"
#include "consts.h"
#include "engine_state.h"

static move_generation_options s_options = { .include_quiet_moves = true, .promotion_types_to_include = PROMOTION_ALL };

static void print_piece(board *board, int8_t x, int8_t y);
static void print_result(game_result result);

void handle_display_command(char *edit_cmd, engine_state state, board *board)
{
    char fen[FEN_LENGTH_UPPER_BOUND + 1], current_move_str[MOVE_DATA_STR_LEN + 1];
    int8_t x, y, move_index;
    game_result result;
    move_generation_result next_moves;
    move_data current_move_data;
    assert(edit_cmd != NULL);
    assert(board != NULL);
    if (is_waiting_for_setup(state) || is_waiting_for_ready(state))
    {
        return;
    }
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd == NULL)
    {
        for (y = RANKS_COUNT - 1; y >= 0; y--)
        {
            for (x = 0; x < FILES_COUNT; x++)
            {
                printf("+---");
            }
            printf("+\n");
            for (x = 0; x < FILES_COUNT; x++)
            {
                printf("| ");
                print_piece(board, x, y);
                printf(" ");
            }
            printf("| %d\n", y + 1);
        }
        for (x = 0; x < FILES_COUNT; x++)
        {
            printf("+---");
        }
        printf("+\n");
        for (x = 0; x < FILES_COUNT; x++)
        {
            printf("  %c ", 'a' + x);
        }
        printf(" \n\n");
        board_to_fen_string(board, fen);
        result = get_game_result(board);
        printf("Fen: %s\n", fen);
        printf("Key: " H64 "\n", board->current_game_state.zobrist_key);
        printf("Result: ");
        print_result(result);
        printf("\n");
        if (result == GR_PLAYING)
        {
            generate_moves(board, &next_moves, s_options);
            printf("Next move(s):");
            for (move_index = 0; move_index < next_moves.moves_count; move_index++)
            {
                current_move_data = move_data_from_move(next_moves.moves[move_index]);
                move_data_to_string(current_move_data, current_move_str);
                printf(" %s", current_move_str);
            }
            printf("\n");
        }
        fflush(stdout);
    }
    else if (strcmp(edit_cmd, GE_CMD_DISPLAY_OPT_FEN) == 0)
    {
        board_to_fen_string(board, fen);
        result = get_game_result(board);
        printf(EG_CMD_FEN " %s " EG_CMD_FEN_OPT_RESULT " ", fen);
        print_result(result);
        if (result == GR_PLAYING)
        {
            generate_moves(board, &next_moves, s_options);
            printf(" " EG_CMD_FEN_OPT_NEXTMOVES);
            for (move_index = 0; move_index < next_moves.moves_count; move_index++)
            {
                current_move_data = move_data_from_move(next_moves.moves[move_index]);
                move_data_to_string(current_move_data, current_move_str);
                printf(" %s", current_move_str);
            }
        }
        printf("\n");
        fflush(stdout);
    }
    else
    {
        return;
    }
}

static void print_piece(board *board, int8_t x, int8_t y)
{
    index_validation_result result;
    piece piece;
    piece_type piece_type;
    bool is_white;
    assert(board != NULL);
    result = compute_index_if_valid(x, y);
    assert(result.valid);
    piece = board->position[result.index];
    piece_type = get_type(piece);
    is_white = get_color(piece) == COLOR_WHITE;
    assert(is_piece_valid(piece));
    switch (piece_type)
    {
        case PIECE_PAWN:
            printf("%s", is_white ? "P" : "p");
            break;
        case PIECE_KNIGHT:
            printf("%s", is_white ? "N" : "n");
            break;
        case PIECE_BISHOP:
            printf("%s", is_white ? "B" : "b");
            break;
        case PIECE_ROOK:
            printf("%s", is_white ? "R" : "r");
            break;
        case PIECE_QUEEN:
            printf("%s", is_white ? "Q" : "q");
            break;
        case PIECE_KING:
            printf("%s", is_white ? "K" : "k");
            break;       
        default:
            printf(" ");
            break;
    }
}

static void print_result(game_result result)
{
    switch (result)
    {
        case GR_PLAYING:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_PLAYING);
            break;
        case GR_WHITE_IS_MATED:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_WHITE_MATED);
            break;
        case GR_BLACK_IS_MATED:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_BLACK_MATED);
            break;
        case GR_STALEMATE:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_STALEMATE);
            break;
        case GR_REPETITION:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_REPETITION);
            break;
        case GR_FIFTY_MOVE_RULE:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_FIFTY_MOVE_RULE);
            break;
        case GR_INSUFFICIENT_MATERIAL:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_INSUFFICIENT_MATERIAL);
            break;
        default:
            printf(EG_CMD_FEN_OPT_RESULT_OPT_UNKNOWN);
            break;
    }
}
