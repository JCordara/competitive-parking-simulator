#cmake_policy(SET CMP0091 NEW)

# Create a Imgui library
file(GLOB IMGUI_SOURCES *.cpp *.h)
add_library(Imgui STATIC ${IMGUI_SOURCES})

# Make sure the compiler can find include files for ImGui library
# when other libraries or executables link to Imgui
target_include_directories (Imgui PUBLIC
    "${CMAKE_SOURCE_DIR}/inc"    
    "${CMAKE_SOURCE_DIR}/inc/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link to C runtime libraries
#set_property(TARGET Imgui PROPERTY MSVC_RUNTIME_LIBRARY 
#    "MultiThreaded$<$<CONFIG:Debug>:Debug>")
