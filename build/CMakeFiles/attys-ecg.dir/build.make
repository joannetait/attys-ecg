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
CMAKE_SOURCE_DIR = /home/jojo/Documents/GitHub/attys-ecg

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jojo/Documents/GitHub/attys-ecg/build

# Include any dependencies generated for this target.
include CMakeFiles/attys-ecg.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/attys-ecg.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/attys-ecg.dir/flags.make

CMakeFiles/attys-ecg.dir/main.cpp.o: CMakeFiles/attys-ecg.dir/flags.make
CMakeFiles/attys-ecg.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jojo/Documents/GitHub/attys-ecg/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/attys-ecg.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/attys-ecg.dir/main.cpp.o -c /home/jojo/Documents/GitHub/attys-ecg/main.cpp

CMakeFiles/attys-ecg.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/attys-ecg.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jojo/Documents/GitHub/attys-ecg/main.cpp > CMakeFiles/attys-ecg.dir/main.cpp.i

CMakeFiles/attys-ecg.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/attys-ecg.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jojo/Documents/GitHub/attys-ecg/main.cpp -o CMakeFiles/attys-ecg.dir/main.cpp.s

# Object files for target attys-ecg
attys__ecg_OBJECTS = \
"CMakeFiles/attys-ecg.dir/main.cpp.o"

# External object files for target attys-ecg
attys__ecg_EXTERNAL_OBJECTS =

attys-ecg: CMakeFiles/attys-ecg.dir/main.cpp.o
attys-ecg: CMakeFiles/attys-ecg.dir/build.make
attys-ecg: CMakeFiles/attys-ecg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jojo/Documents/GitHub/attys-ecg/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable attys-ecg"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/attys-ecg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/attys-ecg.dir/build: attys-ecg

.PHONY : CMakeFiles/attys-ecg.dir/build

CMakeFiles/attys-ecg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/attys-ecg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/attys-ecg.dir/clean

CMakeFiles/attys-ecg.dir/depend:
	cd /home/jojo/Documents/GitHub/attys-ecg/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jojo/Documents/GitHub/attys-ecg /home/jojo/Documents/GitHub/attys-ecg /home/jojo/Documents/GitHub/attys-ecg/build /home/jojo/Documents/GitHub/attys-ecg/build /home/jojo/Documents/GitHub/attys-ecg/build/CMakeFiles/attys-ecg.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/attys-ecg.dir/depend

