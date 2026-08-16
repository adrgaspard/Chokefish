set(MODULE_NAME uci)

add_library(${MODULE_NAME} STATIC
    bestmove_cmd_printer.c
    command_debug.c
    command_display.c
    command_go.c
    command_isready.c
    command_ponderhit.c
    command_position.c
    command_quit.c
    command_setoption.c
    command_stop.c
    command_uci.c
    command_ucinewgame.c
    debug_printer.c
    engine_options.c
    engine_state.c
    search_manager.c
    uci.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
