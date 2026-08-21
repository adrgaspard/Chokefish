set(MODULE_NAME ai)

add_library(${MODULE_NAME} STATIC
    engine.c
    evaluation.c
    evaluation_v1.c
    ordering_v0.c
    ordering_v1.c
    piece_values.c
    search_result.c
    search_utils.c
    search_v0.c
    search_v1.c
    search_v2.c
    time_manager.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
