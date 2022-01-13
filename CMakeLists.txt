cmake_minimum_required(VERSION 3.15.0)
project(CPSC-585-Project)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


#------------------------------- AudioSystem -----------------------------------
# Build AudioSystem as a separate library

add_subdirectory(src/audio)
set(libraries ${libraries} AudioSystem)


# -------------------------------- GLEW ----------------------------------------
# OpenGL Extension Wrangler library
# Make sure you have glew32.dll installed (copy to C:\Windows\System32 folder)
#https://github.com/nigels-com/glew

set(libraries ${libraries} ${CMAKE_SOURCE_DIR}/lib/glew-2.2.0/glew32.lib)


# -------------------------------- GLFW ----------------------------------------
# OpenGL FrameWork library
# https://www.glfw.org/

set(libraries ${libraries} ${CMAKE_SOURCE_DIR}/lib/glfw-3.3.6/glfw3.lib)


# -------------------------------- ImGui ---------------------------------------
# Development GUI library (built separately)
# https://github.com/ocornut/imgui/releases/tag/v1.78

add_subdirectory(lib/imgui-1.78)
set(libraries ${libraries} Imgui)

# -------------------------------- vivid ---------------------------------------
# Console text color library
# https://github.com/gurki/vivid/releases/tag/v3.0.1

set(libraries ${libraries} ${CMAKE_SOURCE_DIR}/lib/vivid-3.0.1/vivid.lib)

# ------------------- Everything else (Header only libraries) ------------------
# Set the "inc" directory as a system include directory (angle brackets)
include_directories(SYSTEM inc)

# ------------------------------------------------------------------------------


# Some CMake magic
# Searches for an OpenGL cmake module that comes with cmake installation
find_package(OpenGL REQUIRED)
set(libraries ${libraries} ${OPENGL_gl_LIBRARY})


# Link to the proper threading libraries depending on the build platform
if(APPLE)
	set(libraries ${libraries} pthread dl)
elseif(UNIX)
	set(libraries ${libraries} pthread GL dl)
elseif(WIN32)
endif()


# Copy all the shaders to build path 
# Tell the build system to re-run CMAKE if one of them changes
file(GLOB files ${CMAKE_SOURCE_DIR}/src/shaders/*)
foreach(file ${files})
	get_filename_component(name ${file} NAME)
	configure_file(${file} shaders/${name})
endforeach()

# Copy all the audio files to build path 
file(GLOB_RECURSE audio_files ${CMAKE_SOURCE_DIR}/res/audio/*)
foreach(file ${audio_files})
	get_filename_component(name ${file} NAME)
	configure_file(${file} audio/${name} COPYONLY)
endforeach()

# Copy all texture files to build path
file(GLOB_RECURSE texture_files ${CMAKE_SOURCE_DIR}/res/textures/*)
foreach(file ${texture_files})
	get_filename_component(name ${file} NAME)
	configure_file(${file} textures/${name} COPYONLY)
endforeach()

# Group source files together
# Good for agile development style
file(GLOB sources src/*)

# Create main application and link libraries
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} ${libraries})


# ------------------------------------------------------------------------------
# Set compiler specific options and stuff
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	list(APPEND _MY_CMAKE_CXX_FLAGS ${_MY_CMAKE_CXX_FLAGS} "-Wall" "-pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	list(APPEND _MY_CMAKE_CXX_FLAGS ${_MY_CMAKE_CXX_FLAGS} "-fdiagnostics-color=always" "-Wall" "-pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options("/permissive-")
	add_compile_options("/Zc:preprocessor") # Causes a warning?
	# Remove the default warning level.
	# https://stackoverflow.com/questions/58708772/cmake-project-in-visual-studio-gives-flag-override-warnings-command-line-warnin
	string(REGEX REPLACE "/W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	 # The above preprocessor warns about certain things in windows headers.
	 # Turn those off: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5105?view=vs-2019
	add_compile_options("/wd5105")

	set(_MY_CMAKE_CXX_FLAGS ${_MY_CMAKE_CXX_FLAGS} /W3)

endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINITIONS})
target_compile_options(${PROJECT_NAME} PRIVATE ${_MY_CMAKE_CXX_FLAGS})
