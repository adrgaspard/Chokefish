#include <string.h>
#include "../core/logging.h"
#include "../core/piece.h"
#include "../core/position.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/consts.h"
#include "commands.h"
#include "consts.h"
#include "engine_state.h"

static void print_piece(board *board, int8_t x, int8_t y);

void handle_display_command(char *edit_cmd, engine_state state, board *board)
{
    char fen[FEN_LENGTH_UPPER_BOUND + 1];
    int8_t x, y;
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
        printf("Fen: %s\n", fen);
        printf("Key: " H64 "\n", board->current_game_state.zobrist_key);
    }
    else if (strcmp(edit_cmd, GE_CMD_DISPLAY_OPT_MINIMAL) == 0)
    {
        for (y = RANKS_COUNT - 1; y >= 0; y--)
        {
            for (x = 0; x < FILES_COUNT; x++)
            {
                print_piece(board, x, y);
            }
            printf("\n");
        }
    }
}

static void print_piece(board *board, int8_t x, int8_t y)
{
    index_validation_result result;
    piece piece;
    piece_type piece_type;
    bool is_white;
    result = compute_index_if_valid(x, y);
    assert(result.valid);
    piece = board->position[result.index];
    piece_type = get_type(piece);
    is_white = get_color(piece) == COLOR_WHITE;
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
