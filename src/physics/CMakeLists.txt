
# Create a library called "PhysicsSystem" from the source files
file(GLOB PHYSICS_SOURCES "./*")
add_library(PhysicsSystem STATIC ${PHYSICS_SOURCES})

# Make sure the compiler can find include files for PhysicsSystem library
# when other libraries or executables link to PhysicsSystem
# And make any files in the src directory accessable as includes
# (none of this #include "../[filename].h" crap)
target_include_directories (PhysicsSystem PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/inc"
    "${CMAKE_SOURCE_DIR}/src"
)

