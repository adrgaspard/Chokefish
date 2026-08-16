set(MODULE_NAME core)

add_library(${MODULE_NAME} STATIC
    bitboard.c
    board.c
    castling.c
    color.c
    enhanced_time.c
    game_state.c
    move.c
    move_generation_result.c
    move_generator.c
    piece.c
    piece_group.c
    position.c
    precomputed_board_data.c
    shared_random.c
    zobrist.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
