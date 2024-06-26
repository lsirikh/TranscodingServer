# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sensorway/Transcoding/TranscodingServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sensorway/Transcoding/TranscodingServer/build

# Include any dependencies generated for this target.
include CMakeFiles/RTSPTranscodingServer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/RTSPTranscodingServer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/RTSPTranscodingServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RTSPTranscodingServer.dir/flags.make

CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/flags.make
CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o: ../main.cpp
CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o -MF CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o.d -o CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o -c /home/sensorway/Transcoding/TranscodingServer/main.cpp

CMakeFiles/RTSPTranscodingServer.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RTSPTranscodingServer.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sensorway/Transcoding/TranscodingServer/main.cpp > CMakeFiles/RTSPTranscodingServer.dir/main.cpp.i

CMakeFiles/RTSPTranscodingServer.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RTSPTranscodingServer.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sensorway/Transcoding/TranscodingServer/main.cpp -o CMakeFiles/RTSPTranscodingServer.dir/main.cpp.s

CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/flags.make
CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o: ../TranscodingService.cpp
CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o -MF CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o.d -o CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o -c /home/sensorway/Transcoding/TranscodingServer/TranscodingService.cpp

CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sensorway/Transcoding/TranscodingServer/TranscodingService.cpp > CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.i

CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sensorway/Transcoding/TranscodingServer/TranscodingService.cpp -o CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.s

CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/flags.make
CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o: ../UriParts.cpp
CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o -MF CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o.d -o CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o -c /home/sensorway/Transcoding/TranscodingServer/UriParts.cpp

CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sensorway/Transcoding/TranscodingServer/UriParts.cpp > CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.i

CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sensorway/Transcoding/TranscodingServer/UriParts.cpp -o CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.s

CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/flags.make
CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o: ../datetime.cpp
CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o -MF CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o.d -o CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o -c /home/sensorway/Transcoding/TranscodingServer/datetime.cpp

CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sensorway/Transcoding/TranscodingServer/datetime.cpp > CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.i

CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sensorway/Transcoding/TranscodingServer/datetime.cpp -o CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.s

CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/flags.make
CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o: ../CustomLogger.cpp
CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o: CMakeFiles/RTSPTranscodingServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o -MF CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o.d -o CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o -c /home/sensorway/Transcoding/TranscodingServer/CustomLogger.cpp

CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sensorway/Transcoding/TranscodingServer/CustomLogger.cpp > CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.i

CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sensorway/Transcoding/TranscodingServer/CustomLogger.cpp -o CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.s

# Object files for target RTSPTranscodingServer
RTSPTranscodingServer_OBJECTS = \
"CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o" \
"CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o" \
"CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o" \
"CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o" \
"CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o"

# External object files for target RTSPTranscodingServer
RTSPTranscodingServer_EXTERNAL_OBJECTS =

RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/main.cpp.o
RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/TranscodingService.cpp.o
RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/UriParts.cpp.o
RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/datetime.cpp.o
RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/CustomLogger.cpp.o
RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/build.make
RTSPTranscodingServer: CMakeFiles/RTSPTranscodingServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable RTSPTranscodingServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RTSPTranscodingServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RTSPTranscodingServer.dir/build: RTSPTranscodingServer
.PHONY : CMakeFiles/RTSPTranscodingServer.dir/build

CMakeFiles/RTSPTranscodingServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RTSPTranscodingServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RTSPTranscodingServer.dir/clean

CMakeFiles/RTSPTranscodingServer.dir/depend:
	cd /home/sensorway/Transcoding/TranscodingServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sensorway/Transcoding/TranscodingServer /home/sensorway/Transcoding/TranscodingServer /home/sensorway/Transcoding/TranscodingServer/build /home/sensorway/Transcoding/TranscodingServer/build /home/sensorway/Transcoding/TranscodingServer/build/CMakeFiles/RTSPTranscodingServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/RTSPTranscodingServer.dir/depend

