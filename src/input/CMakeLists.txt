
# Create a library called "InputSystem" from the source files
file(GLOB INPUT_SOURCES "./*")
add_library(InputSystem STATIC ${INPUT_SOURCES})

# Make sure the compiler can find include files for InputSystem library
# when other libraries or executables link to InputSystem
# And make any files in the src directory accessable as includes
# (none of this #include "../[filename].h" crap)
target_include_directories (InputSystem PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/inc"
    "${CMAKE_SOURCE_DIR}/src"
)

