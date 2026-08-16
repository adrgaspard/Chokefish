cmake_minimum_required(VERSION 3.16)
project(Chokefish C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Linux (mono-config)  : build-linux/bin/
# Windows (multi-config): build-windows/bin/Release/ et build-windows/bin/Debug/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

option(CHOKEFISH_BUILD_TESTSUITE "Build the test suite" ON)
option(CHOKEFISH_ENABLE_LTO "Enable link-time optimization in release builds" ON)

find_package(Threads REQUIRED)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -DDEBUG")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

include(CheckIPOSupported)
if(CHOKEFISH_ENABLE_LTO)
    check_ipo_supported(RESULT CHOKEFISH_IPO_SUPPORTED OUTPUT CHOKEFISH_IPO_ERROR)
    if(CHOKEFISH_IPO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else()
        message(STATUS "LTO not supported by the current toolchain (${CHOKEFISH_IPO_ERROR})")
    endif()
endif()

add_subdirectory(src/modules/core)
add_subdirectory(src/modules/generation)
add_subdirectory(src/modules/serialization)
add_subdirectory(src/modules/game_tools)
if(CHOKEFISH_BUILD_TESTSUITE)
    add_subdirectory(src/modules/test)
endif()
add_subdirectory(src/modules/ai)
add_subdirectory(src/modules/uci)

# Module dependencies
target_link_libraries(generation PRIVATE core Threads::Threads)
target_link_libraries(serialization PRIVATE core)
target_link_libraries(game_tools PRIVATE core serialization)
if(CHOKEFISH_BUILD_TESTSUITE)
    target_link_libraries(test_module PRIVATE core serialization game_tools)
endif()
target_link_libraries(ai PRIVATE core game_tools)
target_link_libraries(uci PRIVATE core serialization game_tools ai)
target_link_libraries(core PUBLIC Threads::Threads)

# Applications
add_subdirectory(src/host)

if(CHOKEFISH_BUILD_TESTSUITE)
    enable_testing()
    add_test(NAME engine_test_suite COMMAND TestSuite)
endif()
