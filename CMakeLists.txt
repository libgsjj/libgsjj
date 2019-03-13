cmake_minimum_required(VERSION 3.10)

# This won't work for Visual Studio (Windows)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: Find a better name
project(libgsjj VERSION 0.5 DESCRIPTION "Library to automatically learn a deterministic finite automaton" LANGUAGES C CXX)

include(GNUInstallDirs)
enable_testing()

add_subdirectory(gsjj)

add_subdirectory(benchmarks)

add_subdirectory(tests)