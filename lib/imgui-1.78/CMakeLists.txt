

# Create a Imgui library
file(GLOB imgui_sources *.cpp)
add_library(Imgui ${imgui_sources})

# Make sure the compiler can find include files for Audio library
# when other libraries or executables link to Imgui
target_include_directories (Imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}
)