# This file is inspired from https://gitlab.kitware.com/cmake/community/wikis/doc/tutorials/How-To-Find-Libraries#writing-find-modules

# - Try to find GMP
# Once done this will define
#  GMP_FOUND - System has GMP
#  GMP_INCLUDE_DIRS - The GMP include directories
#  GMP_LIBRARIES - The libraries needed to use GMP
#  GMP_DEFINITIONS - Compiler switches required for using GMP

find_package(PkgConfig)
pkg_check_modules(PC_GMP QUIET libGMP)
set(GMP_DEFINITIONS ${PC_GMP_CFLAGS_OTHER})

find_path(GMP_INCLUDE_DIR gmpxx.h
          HINTS ${PC_GMP_INCLUDEDIR} ${PC_GMP_INCLUDE_DIRS})

find_library(GMP_LIBRARY NAMES gmp
             HINTS ${PC_GMP_LIBDIR} ${PC_GMP_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GMP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GMP DEFAULT_MSG
                                  GMP_LIBRARY GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY )

set(GMP_LIBRARIES ${GMP_LIBRARY} )
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR} )
