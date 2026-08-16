set(MODULE_NAME serialization)

add_library(${MODULE_NAME} STATIC
    board_data_serializer.c
    move_data.c
    move_data_serializer.c
    position_serializer.c
)
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
