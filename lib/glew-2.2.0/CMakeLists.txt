

# Create glew library
add_library(glew glew.c)

# Make sure the compiler can find include files for glew library
# when other libraries or executables link to glew
target_include_directories (glew PUBLIC ${CMAKE_SOURCE_DIR}/inc)