set(MODULE_NAME test)

add_library(test_module STATIC
    node_exploration_runner.c
    performance_runner.c
    test_utils.c
)
target_include_directories(test_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
