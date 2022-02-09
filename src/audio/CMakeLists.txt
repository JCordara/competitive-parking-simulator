
# Create a library called "AudioManager" from the source files
add_library (AudioManager STATIC
    "Audio.cpp"
    "AudioDevice.cpp"
    "AudioSource.cpp"
    "AudioManager.cpp"
    "OpenALError.cpp"
)

# Link libraries required to build AudioManager
target_link_libraries(AudioManager 
    "${CMAKE_SOURCE_DIR}/lib/openal-soft-1.21.1/$<CONFIG>/OpenAL32.lib"
    "${CMAKE_SOURCE_DIR}/lib/libsndfile-1.0.31/$<CONFIG>/sndfile.lib"
)

# Make sure the compiler can find include files for Audio library
# when other libraries or executables link to AudioManager
# And make any files in the src directory accessable as includes
# (none of this #include "../[filename].h" crap)
target_include_directories (AudioManager PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/inc"
    "${CMAKE_SOURCE_DIR}/src"
)

