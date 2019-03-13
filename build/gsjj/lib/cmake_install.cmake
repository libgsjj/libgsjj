# Install script for directory: /home/gaetan/Github/MAB1/libgsjj/gsjj/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib/libpicosat.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib/liblimboole.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib/libbcsat.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib/libmaple.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/picosat" TYPE FILE FILES
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/picosat/config.h"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/picosat/picosat.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/limboole" TYPE FILE FILES "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/limboole/limboole.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/bcsat" TYPE FILE FILES
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/bc.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/defs.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/gate.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/gatehash.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/handle.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/hashset.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/heap.hh"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/bcsat/timer.hh"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/maple" TYPE FILE FILES
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/simp/SimpSolver.h"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/core/Solver.h"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/core/Dimacs.h"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/core/SolverTypes.h"
    "/home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/utils/System.h"
    )
endif()

