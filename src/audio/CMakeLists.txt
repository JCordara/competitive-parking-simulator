

# Create a library called "AudioSystem" from the source files
add_library (AudioSystem
    Audio.cpp
    AudioDevice.cpp
    AudioSource.cpp
    AudioManager.cpp
    OpenALError.cpp
)

# Link libraries required to build AudioSystem
target_link_libraries(AudioSystem 
    ${CMAKE_SOURCE_DIR}/lib/openal-soft-1.21.1/*.lib
    ${CMAKE_SOURCE_DIR}/lib/libsndfile-1.0.31/sndfile.lib
)

# Make sure the compiler can find include files for Audio library
# when other libraries or executables link to AudioSystem
target_include_directories (AudioSystem PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/inc
)