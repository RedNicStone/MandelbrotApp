# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /home/nic/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/211.7442.42/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/nic/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/211.7442.42/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nic/MandelbrotApp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nic/MandelbrotApp/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ImGuiLib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ImGuiLib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ImGuiLib.dir/flags.make

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.o: CMakeFiles/ImGuiLib.dir/flags.make
CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.o: ../lib/ImGui/imgui.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.o -c /home/nic/MandelbrotApp/lib/ImGui/imgui.cpp

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nic/MandelbrotApp/lib/ImGui/imgui.cpp > CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.i

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nic/MandelbrotApp/lib/ImGui/imgui.cpp -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.s

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.o: CMakeFiles/ImGuiLib.dir/flags.make
CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.o: ../lib/ImGui/imgui_tables.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.o -c /home/nic/MandelbrotApp/lib/ImGui/imgui_tables.cpp

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nic/MandelbrotApp/lib/ImGui/imgui_tables.cpp > CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.i

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nic/MandelbrotApp/lib/ImGui/imgui_tables.cpp -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.s

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.o: CMakeFiles/ImGuiLib.dir/flags.make
CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.o: ../lib/ImGui/imgui_widgets.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.o -c /home/nic/MandelbrotApp/lib/ImGui/imgui_widgets.cpp

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nic/MandelbrotApp/lib/ImGui/imgui_widgets.cpp > CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.i

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nic/MandelbrotApp/lib/ImGui/imgui_widgets.cpp -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.s

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.o: CMakeFiles/ImGuiLib.dir/flags.make
CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.o: ../lib/ImGui/imgui_draw.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.o -c /home/nic/MandelbrotApp/lib/ImGui/imgui_draw.cpp

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nic/MandelbrotApp/lib/ImGui/imgui_draw.cpp > CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.i

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nic/MandelbrotApp/lib/ImGui/imgui_draw.cpp -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.s

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.o: CMakeFiles/ImGuiLib.dir/flags.make
CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.o: ../lib/ImGui/imgui_impl_opengl3.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.o -c /home/nic/MandelbrotApp/lib/ImGui/imgui_impl_opengl3.cpp

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nic/MandelbrotApp/lib/ImGui/imgui_impl_opengl3.cpp > CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.i

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nic/MandelbrotApp/lib/ImGui/imgui_impl_opengl3.cpp -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.s

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.o: CMakeFiles/ImGuiLib.dir/flags.make
CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.o: ../lib/ImGui/imgui_impl_glfw.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.o -c /home/nic/MandelbrotApp/lib/ImGui/imgui_impl_glfw.cpp

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nic/MandelbrotApp/lib/ImGui/imgui_impl_glfw.cpp > CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.i

CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nic/MandelbrotApp/lib/ImGui/imgui_impl_glfw.cpp -o CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.s

# Object files for target ImGuiLib
ImGuiLib_OBJECTS = \
"CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.o" \
"CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.o" \
"CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.o" \
"CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.o" \
"CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.o" \
"CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.o"

# External object files for target ImGuiLib
ImGuiLib_EXTERNAL_OBJECTS =

libImGuiLib.a: CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui.cpp.o
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_tables.cpp.o
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_widgets.cpp.o
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_draw.cpp.o
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_opengl3.cpp.o
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/lib/ImGui/imgui_impl_glfw.cpp.o
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/build.make
libImGuiLib.a: CMakeFiles/ImGuiLib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX static library libImGuiLib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ImGuiLib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ImGuiLib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ImGuiLib.dir/build: libImGuiLib.a

.PHONY : CMakeFiles/ImGuiLib.dir/build

CMakeFiles/ImGuiLib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ImGuiLib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ImGuiLib.dir/clean

CMakeFiles/ImGuiLib.dir/depend:
	cd /home/nic/MandelbrotApp/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nic/MandelbrotApp /home/nic/MandelbrotApp /home/nic/MandelbrotApp/cmake-build-debug /home/nic/MandelbrotApp/cmake-build-debug /home/nic/MandelbrotApp/cmake-build-debug/CMakeFiles/ImGuiLib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ImGuiLib.dir/depend
