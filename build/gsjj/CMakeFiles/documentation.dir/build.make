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

# Utility rule file for documentation.

# Include the progress variables for this target.
include gsjj/CMakeFiles/documentation.dir/progress.make

gsjj/CMakeFiles/documentation: gsjj/Doxyfile.documentation
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/gaetan/Github/MAB1/libgsjj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generate API documentation for documentation"
	cd /home/gaetan/Github/MAB1/libgsjj/gsjj && /usr/bin/cmake -E make_directory /home/gaetan/Github/MAB1/libgsjj/build/gsjj/documentation
	cd /home/gaetan/Github/MAB1/libgsjj/gsjj && /usr/bin/doxygen /home/gaetan/Github/MAB1/libgsjj/build/gsjj/Doxyfile.documentation

documentation: gsjj/CMakeFiles/documentation
documentation: gsjj/CMakeFiles/documentation.dir/build.make

.PHONY : documentation

# Rule to build all files generated by this target.
gsjj/CMakeFiles/documentation.dir/build: documentation

.PHONY : gsjj/CMakeFiles/documentation.dir/build

gsjj/CMakeFiles/documentation.dir/clean:
	cd /home/gaetan/Github/MAB1/libgsjj/build/gsjj && $(CMAKE_COMMAND) -P CMakeFiles/documentation.dir/cmake_clean.cmake
.PHONY : gsjj/CMakeFiles/documentation.dir/clean

gsjj/CMakeFiles/documentation.dir/depend:
	cd /home/gaetan/Github/MAB1/libgsjj/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gaetan/Github/MAB1/libgsjj /home/gaetan/Github/MAB1/libgsjj/gsjj /home/gaetan/Github/MAB1/libgsjj/build /home/gaetan/Github/MAB1/libgsjj/build/gsjj /home/gaetan/Github/MAB1/libgsjj/build/gsjj/CMakeFiles/documentation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gsjj/CMakeFiles/documentation.dir/depend

