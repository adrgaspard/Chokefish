set(MODULE_NAME generation)

add_library(${MODULE_NAME} STATIC
    generation_scheduler.c
    magic_generator.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
