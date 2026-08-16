set(MODULE_NAME game_tools)

add_library(${MODULE_NAME} STATIC
    board_utils.c
    game_result.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
