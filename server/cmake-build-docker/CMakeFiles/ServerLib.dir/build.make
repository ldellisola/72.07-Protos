# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /tmp/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/server/cmake-build-docker

# Include any dependencies generated for this target.
include CMakeFiles/ServerLib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ServerLib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ServerLib.dir/flags.make

CMakeFiles/ServerLib.dir/src/main.c.o: CMakeFiles/ServerLib.dir/flags.make
CMakeFiles/ServerLib.dir/src/main.c.o: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/server/cmake-build-docker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ServerLib.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ServerLib.dir/src/main.c.o   -c /tmp/server/src/main.c

CMakeFiles/ServerLib.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ServerLib.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/server/src/main.c > CMakeFiles/ServerLib.dir/src/main.c.i

CMakeFiles/ServerLib.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ServerLib.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/server/src/main.c -o CMakeFiles/ServerLib.dir/src/main.c.s

CMakeFiles/ServerLib.dir/src/cli.c.o: CMakeFiles/ServerLib.dir/flags.make
CMakeFiles/ServerLib.dir/src/cli.c.o: ../src/cli.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/server/cmake-build-docker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ServerLib.dir/src/cli.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ServerLib.dir/src/cli.c.o   -c /tmp/server/src/cli.c

CMakeFiles/ServerLib.dir/src/cli.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ServerLib.dir/src/cli.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/server/src/cli.c > CMakeFiles/ServerLib.dir/src/cli.c.i

CMakeFiles/ServerLib.dir/src/cli.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ServerLib.dir/src/cli.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/server/src/cli.c -o CMakeFiles/ServerLib.dir/src/cli.c.s

# Object files for target ServerLib
ServerLib_OBJECTS = \
"CMakeFiles/ServerLib.dir/src/main.c.o" \
"CMakeFiles/ServerLib.dir/src/cli.c.o"

# External object files for target ServerLib
ServerLib_EXTERNAL_OBJECTS =

libServerLib.a: CMakeFiles/ServerLib.dir/src/main.c.o
libServerLib.a: CMakeFiles/ServerLib.dir/src/cli.c.o
libServerLib.a: CMakeFiles/ServerLib.dir/build.make
libServerLib.a: CMakeFiles/ServerLib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/server/cmake-build-docker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libServerLib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ServerLib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ServerLib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ServerLib.dir/build: libServerLib.a

.PHONY : CMakeFiles/ServerLib.dir/build

CMakeFiles/ServerLib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ServerLib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ServerLib.dir/clean

CMakeFiles/ServerLib.dir/depend:
	cd /tmp/server/cmake-build-docker && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/server /tmp/server /tmp/server/cmake-build-docker /tmp/server/cmake-build-docker /tmp/server/cmake-build-docker/CMakeFiles/ServerLib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ServerLib.dir/depend

