cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME Chokefish)
project(${PROJECT_NAME})
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(APPLICATIONS_TO_INCLUDE
    ${PROJECT_NAME} "src/host/app.c" "core,serialization,game_tools,ai,uci"
    Generator "src/host/generator.c" "core,generation"
    TestSuite "src/host/test.c" "core,serialization,game_tools,test"
)
set(MODULES_TO_INCLUDE
    core NULL
    generation "core"
    serialization "core"
    game_tools "core,serialization"
    test "core,serialization,game_tools"
    ai "core,game_tools"
    uci "core,serialization,game_tools,ai"
)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -DDEBUG")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
endif()

set(MAP_COUNTER 0)
foreach(MODULE ${MODULES_TO_INCLUDE})
    if(MAP_COUNTER EQUAL 0)
        add_subdirectory(src/modules/${MODULE})
        set(MAP_COUNTER 1)
    else()
        set(MAP_COUNTER 0)
    endif()
endforeach()

set(MAP_COUNTER 0)
foreach(ITEM ${MODULES_TO_INCLUDE})
    if(MAP_COUNTER EQUAL 1)
        string(REPLACE "," ";" MODULE_DEPENDENCIES ${ITEM})
        if(NOT MODULE_DEPENDENCIES STREQUAL NULL)
            foreach(DEPENDENCY ${MODULE_DEPENDENCIES})
                target_link_libraries(${MODULE_NAME} ${DEPENDENCY})
            endforeach()
        endif()
        set(MAP_COUNTER 0)
    else()
        set(MODULE_NAME ${ITEM})
        set(MAP_COUNTER 1)
    endif()
endforeach()

set(MAP_COUNTER 0)
foreach(ITEM ${APPLICATIONS_TO_INCLUDE})
    if(MAP_COUNTER EQUAL 2)
        string(REPLACE "," ";" APPLICATION_DEPENDENCIES ${ITEM})
        add_executable(${APPLICATION_NAME} ${APPLICATION_MAIN})
        if(NOT APPLICATION_DEPENDENCIES STREQUAL NULL)
            foreach(DEPENDENCY ${APPLICATION_DEPENDENCIES})
                target_link_libraries(${APPLICATION_NAME} ${DEPENDENCY})
            endforeach()
        endif()
        set(MAP_COUNTER 0)
    elseif(MAP_COUNTER EQUAL 1)
        set(APPLICATION_MAIN ${ITEM})
        set(MAP_COUNTER 2)
    else()
        set(APPLICATION_NAME ${ITEM})
        set(MAP_COUNTER 1)
    endif()
endforeach()
