
# Create a library called "EntityComponent" from the source files
file(GLOB COMPONENT_SOURCES "./*.cpp")
add_library(EntityComponent STATIC ${COMPONENT_SOURCES} "LightingComponent.cpp")

# Make sure the compiler can find include files for Entity Component library
# when other libraries or executables link to EntityComponent
# And make any files in the src directory accessable as includes
# (none of this #include "../[filename].h" crap)
target_include_directories (EntityComponent PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/inc"
    "${CMAKE_SOURCE_DIR}/src"
)

