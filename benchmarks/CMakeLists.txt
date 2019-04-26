option(BUILD_BENCHMARKS "Whether to build the benchmarks" OFF)

if(BUILD_BENCHMARKS)
    add_executable(benchmarks main.cpp LFDFA.cpp)
    target_link_libraries(benchmarks gsjj)
endif(BUILD_BENCHMARKS)