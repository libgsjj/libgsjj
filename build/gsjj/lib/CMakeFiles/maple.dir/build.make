# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gaetan/Github/MAB1/libgsjj

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gaetan/Github/MAB1/libgsjj/build

# Include any dependencies generated for this target.
include gsjj/lib/CMakeFiles/maple.dir/depend.make

# Include the progress variables for this target.
include gsjj/lib/CMakeFiles/maple.dir/progress.make

# Include the compile flags for this target's objects.
include gsjj/lib/CMakeFiles/maple.dir/flags.make

gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.o: gsjj/lib/CMakeFiles/maple.dir/flags.make
gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.o: ../gsjj/lib/MapleCOMSPS_LRB/core/Solver.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gaetan/Github/MAB1/libgsjj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.o"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.o -c /home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/core/Solver.cc

gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.i"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/core/Solver.cc > CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.i

gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.s"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/core/Solver.cc -o CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.s

gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.o: gsjj/lib/CMakeFiles/maple.dir/flags.make
gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.o: ../gsjj/lib/MapleCOMSPS_LRB/simp/SimpSolver.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gaetan/Github/MAB1/libgsjj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.o"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.o -c /home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/simp/SimpSolver.cc

gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.i"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/simp/SimpSolver.cc > CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.i

gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.s"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gaetan/Github/MAB1/libgsjj/gsjj/lib/MapleCOMSPS_LRB/simp/SimpSolver.cc -o CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.s

# Object files for target maple
maple_OBJECTS = \
"CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.o" \
"CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.o"

# External object files for target maple
maple_EXTERNAL_OBJECTS =

gsjj/lib/libmaple.a: gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/core/Solver.cc.o
gsjj/lib/libmaple.a: gsjj/lib/CMakeFiles/maple.dir/MapleCOMSPS_LRB/simp/SimpSolver.cc.o
gsjj/lib/libmaple.a: gsjj/lib/CMakeFiles/maple.dir/build.make
gsjj/lib/libmaple.a: gsjj/lib/CMakeFiles/maple.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gaetan/Github/MAB1/libgsjj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libmaple.a"
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && $(CMAKE_COMMAND) -P CMakeFiles/maple.dir/cmake_clean_target.cmake
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/maple.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
gsjj/lib/CMakeFiles/maple.dir/build: gsjj/lib/libmaple.a

.PHONY : gsjj/lib/CMakeFiles/maple.dir/build

gsjj/lib/CMakeFiles/maple.dir/clean:
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib && $(CMAKE_COMMAND) -P CMakeFiles/maple.dir/cmake_clean.cmake
.PHONY : gsjj/lib/CMakeFiles/maple.dir/clean

gsjj/lib/CMakeFiles/maple.dir/depend:
	cd /home/gaetan/Github/MAB1/libgsjj/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gaetan/Github/MAB1/libgsjj /home/gaetan/Github/MAB1/libgsjj/gsjj/lib /home/gaetan/Github/MAB1/libgsjj/build /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib /home/gaetan/Github/MAB1/libgsjj/build/gsjj/lib/CMakeFiles/maple.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gsjj/lib/CMakeFiles/maple.dir/depend

