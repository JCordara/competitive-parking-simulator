# Allow for multiple versions of CMake (we should use cmake 3.22 tho)
cmake_minimum_required(VERSION 3.15...3.22)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
endif()

# Set this specific policy in order to link against C runtime libraries
cmake_policy(SET CMP0091 NEW)

# Project name
project(CPSC_585_Project VERSION 0.1 LANGUAGES C CXX)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Link to C/C++ runtime library statically
# Note: Still seems to link to vcruntime140_1.dll which may not be installed on
# all users systems. Current fix is to copy the shared library to executable 
# directory, which is apparently bad/unsafe or something
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


#------------------------------- AudioSystem -----------------------------------
# Build AudioSystem as a separate library

add_subdirectory(src/audio)
list(APPEND LIBRARIES AudioSystem)


#------------------------- Entities and Components -----------------------------
# Build Entity/Component system as a separate library

add_subdirectory(src/components)
list(APPEND LIBRARIES EntityComponent)


#--------------------------------- PhysX ---------------------------------------
# Physics library
# https://github.com/NVIDIAGameWorks/PhysX

list(APPEND LIBRARIES
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/LowLevel_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/LowLevelAABB_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/LowLevelDynamics_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysX_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXCharacterKinematic_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXCommon_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXCooking_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXExtensions_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXFoundation_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXPvdSDK_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXTask_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/PhysXVehicle_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SampleBase_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SampleFramework_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SamplesToolkit_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SceneQuery_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SimulationController_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SnippetRender_static_64.lib"
	"${CMAKE_SOURCE_DIR}/lib/physx-4.1/$<CONFIG>/SnippetUtils_static_64.lib"
)


# -------------------------------- GLEW ----------------------------------------
# OpenGL Extension Wrangler library
# https://github.com/nigels-com/glew

add_subdirectory(lib/glew-2.2.0)
list(APPEND LIBRARIES glew)


# -------------------------------- ImGui ---------------------------------------
# Development GUI library (built separately)
# https://github.com/ocornut/imgui/releases/tag/v1.78

add_subdirectory(lib/imgui-1.78)
list(APPEND LIBRARIES Imgui)


# -------------------------------- GLFW ----------------------------------------
# OpenGL FrameWork library
# https://www.glfw.org/

list(APPEND LIBRARIES
	"${CMAKE_SOURCE_DIR}/lib/glfw-3.3.6/$<CONFIG>/glfw3.lib")


# -------------------------------- vivid ---------------------------------------
# Console text color library
# https://github.com/gurki/vivid/releases/tag/v3.0.1

list(APPEND LIBRARIES
	"${CMAKE_SOURCE_DIR}/lib/vivid-3.0.1/$<CONFIG>/vivid.lib")


# -------------------------------- Assimp -------------------------------------
# Open Asset Import Library 

list(APPEND LIBRARIES
	"${CMAKE_SOURCE_DIR}/lib/assimp/$<CONFIG>/assimp-vc142-mt.lib")


# Some CMake magic
# Searches for an OpenGL cmake module that comes with cmake installation
find_package(OpenGL REQUIRED)
list(APPEND LIBRARIES ${OPENGL_gl_LIBRARY})


# Link to the proper threading libraries depending on the build platform
if(APPLE)
	list(APPEND LIBRARIES pthread dl)
elseif(UNIX)
	list(APPEND LIBRARIES pthread GL dl)
elseif(WIN32)
	# Nothing to do here 
endif()

# Group source files together
# Good for agile development style
file(GLOB SOURCES "src/*")

# Create main application and link libraries
set(APP_NAME "CompetitiveParkingSimulator")
add_executable(${APP_NAME} "${SOURCES}")


# Set the "inc" directory as a system include directory (angle brackets)
target_include_directories(${APP_NAME} SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/inc")

# Link the libraries listed above
target_link_libraries(${APP_NAME} "${LIBRARIES}")

# Function to copy files to the build binary output directory
function(TO_BINARY_DIR SOURCE_DIR DEST_DIR)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/${SOURCE_DIR}"
		"$<TARGET_FILE_DIR:${APP_NAME}>${DEST_DIR}")
endfunction()

# Copy all the shaders to binary output dir on build
to_binary_dir("src/shaders" "/shaders")

# Copy all the audio files to binary output dir on build
to_binary_dir("res/audio" "/audio")

# Copy all the textures to binary output dir on build
to_binary_dir("res/textures" "/textures")

# Copy all the models to binary output dir on build
to_binary_dir("res/models" "/models")

# Copy all the shared libraries to binary output dir on build
to_binary_dir("lib/shared/$<CONFIG>" "")


#                   Set compiler specific options and stuff                    #
# -----------------------------------------------------------------------------#

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	list(APPEND TARGET_COMPILE_OPTIONS ${TARGET_COMPILE_OPTIONS} "-Wall" "-pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	list(APPEND TARGET_COMPILE_OPTIONS "-fdiagnostics-color=always" "-Wall" "-pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# Suppress some linker errors
	list(APPEND TARGET_LINK_OPTIONS "/NODEFAULTLIB:LIBCMT")
	# Run without console when in release mode
	list(APPEND TARGET_LINK_OPTIONS "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>")
	list(APPEND TARGET_LINK_OPTIONS "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>")
	# Make IDE underline non-standard-conforming code
	list(APPEND TARGET_COMPILE_OPTIONS "/permissive-")
	list(APPEND TARGET_COMPILE_OPTIONS "/Zc:preprocessor") # Causes a warning?
	list(APPEND TARGET_COMPILE_OPTIONS "/MP") # Build using all available cores
	# Remove the default warning level.
	# https://stackoverflow.com/questions/58708772/cmake-project-in-visual-studio-gives-flag-override-warnings-command-line-warnin
	string(REGEX REPLACE "/W[3|4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	# The above preprocessor warns about certain things in windows headers.
	# Turn those off: https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5105?view=vs-2019
	list(APPEND TARGET_COMPILE_OPTIONS "/wd5105")
	list(APPEND TARGET_COMPILE_OPTIONS "/W3")
endif()

target_link_options(${APP_NAME} PRIVATE ${TARGET_LINK_OPTIONS})
target_compile_definitions(${APP_NAME} PRIVATE ${DEFINITIONS})
target_compile_options(${APP_NAME} PRIVATE ${TARGET_COMPILE_OPTIONS})
set_target_properties(${APP_NAME} PROPERTIES INSTALL_RPATH "./" BUILD_RPATH "./")
