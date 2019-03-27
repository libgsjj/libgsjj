set(MAPLE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/MapleCOMSPS_LRB)
set(PICOSAT_SOLVER ${CMAKE_CURRENT_SOURCE_DIR}/picosat)
set(LIMBOOLE_SOLVER ${CMAKE_CURRENT_SOURCE_DIR}/limboole)
set(BCMINISAT220_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bcsat)

# PicoSAT library (needed for limboole)
file(GLOB PICOSAT_SOURCES
    ${PICOSAT_SOLVER}/picosat.c
)
set(PICOSAT_HEADERS ${PICOSAT_SOLVER}/..)
add_library(picosat STATIC ${PICOSAT_SOURCES})
target_include_directories(picosat
    PUBLIC
        $<BUILD_INTERFACE:${PICOSAT_HEADERS}>
        $<INSTALL_INTERFACE:include>
)
target_compile_options(picosat
    PRIVATE
        "-fPIC"
)

# Limboole
file(GLOB LIMBOOLE_SOURCES
    ${LIMBOOLE_SOLVER}/limboole.c
)
set(LIMBOOLE_HEADERS ${LIMBOOLE_SOLVER}/..)
add_library(limboole STATIC ${LIMBOOLE_SOURCES})
target_include_directories(limboole
    PUBLIC
        $<BUILD_INTERFACE:${LIMBOOLE_HEADERS}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(limboole picosat)
target_compile_options(limboole
    PRIVATE
        # Needed for limboole
        "-DLIMBOOLE_USE_PICOSAT"
        "-DVERSION='cat ${LIMBOOLE_SOLVER}/VERSION'"
        # To be able to build a static library
        "-fPIC"
)

# Bison and flex (needed for bcsat)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# The bcsat front-end for Minisat (MapleCOMSPS_LRB)
# This front-end allows to use any boolean formula instead of just CNF
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bcsat)
BISON_TARGET(bcsat_parser
    ${BCMINISAT220_PATH}/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/bcsat/parser.cc
    COMPILE_FLAGS "-b parser -p bcp_ -d"
)
BISON_TARGET(bcsat_parser11
    ${BCMINISAT220_PATH}/parser11.y
    ${CMAKE_CURRENT_BINARY_DIR}/bcsat/parser11.cc
    COMPILE_FLAGS "-b parser11 -p bcp11_ -d"
)
FLEX_TARGET(bcsat_lexer
    ${BCMINISAT220_PATH}/lexer.lex
    ${CMAKE_CURRENT_BINARY_DIR}/bcsat/lexer.cc
    COMPILE_FLAGS "-L -Pbcp_"
)
FLEX_TARGET(bcsat_lexer11
    ${BCMINISAT220_PATH}/lexer11.lex
    ${CMAKE_CURRENT_BINARY_DIR}/bcsat/lexer11.cc
    COMPILE_FLAGS "-L -Pbcp11_"
)
ADD_FLEX_BISON_DEPENDENCY(bcsat_lexer bcsat_parser)
ADD_FLEX_BISON_DEPENDENCY(bcsat_lexer11 bcsat_parser11)

set(BCMINISAT_SOURCES
    ${BCMINISAT220_PATH}/bcminisat220_solve.cc
    ${BCMINISAT220_PATH}/defs.cc
    ${BCMINISAT220_PATH}/bc.cc
    ${BCMINISAT220_PATH}/gate.cc
    ${BCMINISAT220_PATH}/gatehash.cc
    ${BCMINISAT220_PATH}/handle.cc
    ${BCMINISAT220_PATH}/timer.cc
    ${BCMINISAT220_PATH}/heap.cc
    ${BCMINISAT220_PATH}/defs.hh
    ${BCMINISAT220_PATH}/bc.hh
    ${BCMINISAT220_PATH}/gate.hh
    ${BCMINISAT220_PATH}/gatehash.hh
    ${BCMINISAT220_PATH}/handle.hh
    ${BCMINISAT220_PATH}/timer.hh
    ${BCMINISAT220_PATH}/heap.hh
    ${BCMINISAT220_PATH}/hashset.hh
    ${BISON_bcsat_parser_OUTPUTS}
    ${BISON_bcsat_parser11_OUTPUTS}
    ${FLEX_bcsat_lexer_OUTPUTS}
    ${FLEX_bcsat_lexer11_OUTPUTS}
)
set(BCMINISAT_HEADERS ${BCMINISAT220_PATH}/..)
add_library(bcsat STATIC ${BCMINISAT_SOURCES})
target_include_directories(bcsat
    PUBLIC
        $<BUILD_INTERFACE:
            ${MAPLE_PATH}/..
            ${BCMINISAT_HEADERS}
        >
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_options(bcsat
    PUBLIC
        "-DBC_HAS_MINISAT"
        "-DMINISAT220SIMP"
    PRIVATE
        "-fPIC"
)

# Files needed to compile the SAT solver
file(GLOB MAPLE_SOURCES
    ${MAPLE_PATH}/simp/SimpSolver.cc
    ${MAPLE_PATH}/core/Solver.cc
)
set(MAPLE_HEADERS ${MAPLE_PATH}/..)
add_library(maple STATIC ${MAPLE_SOURCES})
target_include_directories(maple
    PUBLIC
        $<BUILD_INTERFACE: ${MAPLE_HEADERS}>
        $<INSTALL_INTERFACE:include>
)
target_compile_options(maple
    PRIVATE
        "-fPIC"
        "-DNDEBUG" # We disable the debug features of maple
)

# Installation of the dependencies (when using make install)
install(TARGETS picosat limboole bcsat maple
    ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
# Copy the header files (it's necessary since our header files use them)
install(FILES
    picosat/config.h
    picosat/picosat.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/picosat
)
install(FILES
    limboole/limboole.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/limboole
)
install(FILES
    bcsat/bc.hh
    bcsat/defs.hh
    bcsat/gate.hh
    bcsat/gatehash.hh
    bcsat/handle.hh
    bcsat/hashset.hh
    bcsat/heap.hh
    bcsat/timer.hh
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bcsat
)
install(FILES
    MapleCOMSPS_LRB/core/Solver.h
    MapleCOMSPS_LRB/core/Dimacs.h
    MapleCOMSPS_LRB/core/SolverTypes.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MapleCOMSPS_LRB/core
)
install(FILES
    MapleCOMSPS_LRB/mtl/Alg.h
    MapleCOMSPS_LRB/mtl/Alloc.h
    MapleCOMSPS_LRB/mtl/Heap.h
    MapleCOMSPS_LRB/mtl/IntTypes.h
    MapleCOMSPS_LRB/mtl/Map.h
    MapleCOMSPS_LRB/mtl/Queue.h
    MapleCOMSPS_LRB/mtl/Sort.h
    MapleCOMSPS_LRB/mtl/Vec.h
    MapleCOMSPS_LRB/mtl/XAlloc.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MapleCOMSPS_LRB/mtl
)
install(FILES
    MapleCOMSPS_LRB/simp/SimpSolver.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MapleCOMSPS_LRB/simp
)
install(FILES
    MapleCOMSPS_LRB/utils/System.h
    MapleCOMSPS_LRB/utils/Options.h
    MapleCOMSPS_LRB/utils/ParseUtils.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MapleCOMSPS_LRB/utils
)