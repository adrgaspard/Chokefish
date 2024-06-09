#include "types.h"

DECLARE_DYNAMIC_ARRAY_IMPLEMENTATION(bitboard, bitboard_dynamic_array)
DECLARE_STATIC_STACK_IMPLEMENTATION(game_state, 10000, game_state_stack)
DECLARE_STATIC_STACK_IMPLEMENTATION(move, 10000, move_stack)
