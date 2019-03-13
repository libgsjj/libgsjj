# This file is inspired from https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/How-To-Find-Libraries#writing-find-modules

# - Try to find cvc4
# Once done this will define
#  cvc4_FOUND - System has cvc4
#  cvc4_INCLUDE_DIRS - The cvc4 include directories
#  cvc4_LIBRARIES - The libraries needed to use cvc4
#  cvc4_DEFINITIONS - Compiler switches required for using cvc4

find_package(PkgConfig)
pkg_check_modules(PC_cvc4 QUIET libcvc4)
set(cvc4_DEFINITIONS ${PC_cvc4_CFLAGS_OTHER})

find_path(cvc4_INCLUDE_DIR cvc4/cvc4.h
          HINTS ${PC_cvc4_INCLUDEDIR} ${PC_cvc4_INCLUDE_DIRS}
          PATH_SUFFIXES cvc4 )

find_library(cvc4_LIBRARY NAMES cvc4
             HINTS ${PC_cvc4_LIBDIR} ${PC_cvc4_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set cvc4_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(cvc4 DEFAULT_MSG
                                  cvc4_LIBRARY cvc4_INCLUDE_DIR)

mark_as_advanced(cvc4_INCLUDE_DIR cvc4_LIBRARY )

set(cvc4_LIBRARIES ${cvc4_LIBRARY} )
set(cvc4_INCLUDE_DIRS ${cvc4_INCLUDE_DIR} )
