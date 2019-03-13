option(BUILD_TESTS "Whether to build the tests" OFF)

if (BUILD_TESTS)
    set(TESTS_SOURCES
        main.cpp
        utils.cpp
        deterministicTests.cpp
        binaryConversion.cpp
    )

    add_executable(tests ${TESTS_SOURCES})
    target_link_libraries(tests gsjj)
    add_test(tests tests)
endif(BUILD_TESTS)