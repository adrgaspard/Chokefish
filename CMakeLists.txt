cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME Chokefish)
project(${PROJECT_NAME})
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(MODULES_TO_INCLUDE
    core
    # serialization
    # ai
    # uci
)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O1 -DDEBUG")

add_subdirectory(src/host)
foreach(MODULE ${MODULES_TO_INCLUDE})
    add_subdirectory(src/modules/${MODULE})
endforeach()

add_executable(${PROJECT_NAME} src/host/main.c)
foreach(MODULE ${MODULES_TO_INCLUDE})
    target_link_libraries(${PROJECT_NAME} ${MODULE})
endforeach()