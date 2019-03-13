option(BUILD_BENCHMARKS "Whether to build the benchmarks" OFF)

if(BUILD_BENCHMARKS)
    # To easily paralellise the different methods
    find_package(OpenMP REQUIRED)

    add_executable(benchmarks main.cpp)
    target_link_libraries(benchmarks
        gsjj
        ${OpenMP_CXX_LIBRARIES}
    )
endif(BUILD_BENCHMARKS)