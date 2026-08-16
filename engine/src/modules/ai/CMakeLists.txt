set(MODULE_NAME ai)

add_library(${MODULE_NAME} STATIC
    engine.c
    evaluation.c
    search_result.c
    search_v0.c
    time_manager.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
