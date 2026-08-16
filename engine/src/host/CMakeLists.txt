add_executable(Chokefish app.c)
target_link_libraries(Chokefish PRIVATE core serialization game_tools ai uci)

add_executable(Generator generator.c)
target_link_libraries(Generator PRIVATE core generation)

if(CHOKEFISH_BUILD_TESTSUITE)
    add_executable(TestSuite test.c)
    target_link_libraries(TestSuite PRIVATE core serialization game_tools test_module)
endif()
