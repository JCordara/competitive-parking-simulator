#cmake_policy(SET CMP0091 NEW)

# Create glew library
file(GLOB GLEW_SOURCES glew.c ${CMAKE_SOURCE_DIR}/inc/GL/*.h)
add_library(glew STATIC ${GLEW_SOURCES})

# Make sure the compiler can find include files for glew library
# when other libraries or executables link to glew
target_include_directories (glew PUBLIC ${CMAKE_SOURCE_DIR}/inc)

# Link to C runtime libraries
#set_property(TARGET glew PROPERTY MSVC_RUNTIME_LIBRARY 
#    "MultiThreaded$<$<CONFIG:Debug>:Debug>")
