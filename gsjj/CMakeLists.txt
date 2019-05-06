# Boost
find_package(Boost REQUIRED COMPONENTS program_options)
# To find the module for cvc4
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# cvc4
find_package(cvc4 REQUIRED)
# GMP is required for cvc4
find_package(GMP REQUIRED)
# Required for one of the dependencies
find_package(ZLIB REQUIRED)
# Needed to use std::thread
find_package(Threads REQUIRED)

add_subdirectory(lib)

set(SOURCE_FILES
    src/binary.cpp
    src/passive/Method.cpp
    src/passive/utils.cpp
    src/passive/MethodFactory.cpp
    src/passive/CNF/BinaryCNFMethod.cpp
    src/passive/CNF/HeuleVerwerCNFMethod.cpp
    src/passive/CNF/SATCNFMethod.cpp
    src/passive/CNF/UnaryCNFMethod.cpp
    src/passive/NonCNF/BinaryNonCNFMethod.cpp
    src/passive/NonCNF/HeuleVerwerNonCNFMethod.cpp
    src/passive/NonCNF/SATNonCNFMethod.cpp
    src/passive/NonCNF/UnaryNonCNFMethod.cpp
    src/passive/SMT/BiermannMethod.cpp
    src/passive/SMT/NeiderJansenMethod.cpp
    src/passive/SMT/SMTMethod.cpp
)

add_library(gsjj SHARED
    ${SOURCE_FILES}
)

set(BUILD_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${cvc4_INCLUDE_DIRS}
    ${GMP_INCLUDE_DIRS}
)

target_include_directories(gsjj
    PUBLIC
        "$<BUILD_INTERFACE:${BUILD_HEADERS}>"
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)
target_link_libraries(gsjj
    PUBLIC
        ${Boost_LIBRARIES}
        ${cvc4_LIBRARIES}
        ${GMP_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
        picosat
        limboole
        maple
        bcsat
)
target_compile_options(gsjj
    PRIVATE
        $<IF:$<CXX_COMPILER_ID:MSVC>,
            # For Visual Code compiler
            "/W4"
            $<$<CONFIG:RELEASE>: "/o2" >,
            # For the other compilers
            "-pedantic"
            "-Wall"
            "-Wextra"
            $<$<CONFIG:RELEASE>: "-O2" >
        >
)
install(TARGETS gsjj
    EXPORT gsjjConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# Since using PUBLIC_HEADER in the install(TARGETS ...) does not copy the directory structure, we explicitly copy the include directory
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT gsjjConfig DESTINATION share/gsjj/cmake)

export(TARGETS gsjj FILE gsjjConfig.cmake)
 
# Documentation
option(BUILD_DOCUMENTATION "Whether to build the documentation" OFF)
if(BUILD_DOCUMENTATION)
    set(DOXYGEN_OUTPUT_DIRECTORY documentation)
    set(DOXYGEN_JAVADOC_AUTOBRIEF TRUE)
    set(DOXYGEN_SOURCE_BROWSER TRUE)
    set(DOXYGEN_EXTRACT_ALL TRUE)
    set(DOXYGEN_TOC_INCLUDE_HEADINGS 4)
    find_package(Doxygen
        REQUIRED dot
        OPTIONAL_COMPONENTS mscgen dia)

    doxygen_add_docs(documentation
        ../README.md
        include
        src
    )
endif(BUILD_DOCUMENTATION)
