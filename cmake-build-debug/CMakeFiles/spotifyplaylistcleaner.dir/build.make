# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Users/tamimnorani/Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake

# The command to remove a file.
RM = /Users/tamimnorani/Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tamimnorani/projects/personal/spotifyplaylistcleaner

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/spotifyplaylistcleaner.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/spotifyplaylistcleaner.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/spotifyplaylistcleaner.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/spotifyplaylistcleaner.dir/flags.make

CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o: CMakeFiles/spotifyplaylistcleaner.dir/flags.make
CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o: /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/main.cpp
CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o: CMakeFiles/spotifyplaylistcleaner.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o -MF CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o.d -o CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o -c /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/main.cpp

CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/main.cpp > CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.i

CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/main.cpp -o CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.s

# Object files for target spotifyplaylistcleaner
spotifyplaylistcleaner_OBJECTS = \
"CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o"

# External object files for target spotifyplaylistcleaner
spotifyplaylistcleaner_EXTERNAL_OBJECTS =

spotifyplaylistcleaner: CMakeFiles/spotifyplaylistcleaner.dir/main.cpp.o
spotifyplaylistcleaner: CMakeFiles/spotifyplaylistcleaner.dir/build.make
spotifyplaylistcleaner: CMakeFiles/spotifyplaylistcleaner.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable spotifyplaylistcleaner"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/spotifyplaylistcleaner.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/spotifyplaylistcleaner.dir/build: spotifyplaylistcleaner
.PHONY : CMakeFiles/spotifyplaylistcleaner.dir/build

CMakeFiles/spotifyplaylistcleaner.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/spotifyplaylistcleaner.dir/cmake_clean.cmake
.PHONY : CMakeFiles/spotifyplaylistcleaner.dir/clean

CMakeFiles/spotifyplaylistcleaner.dir/depend:
	cd /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tamimnorani/projects/personal/spotifyplaylistcleaner /Users/tamimnorani/projects/personal/spotifyplaylistcleaner /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug /Users/tamimnorani/projects/personal/spotifyplaylistcleaner/cmake-build-debug/CMakeFiles/spotifyplaylistcleaner.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/spotifyplaylistcleaner.dir/depend
