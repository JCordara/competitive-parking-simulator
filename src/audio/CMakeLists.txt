
# Create a library called "AudioSystem" from the source files
add_library (AudioSystem STATIC
    "Audio.cpp"
    "AudioDevice.cpp"
    "AudioSource.cpp"
    "AudioSystem.cpp"
    "OpenALError.cpp"
)

# Link libraries required to build AudioSystem
target_link_libraries(AudioSystem 
    "${CMAKE_SOURCE_DIR}/lib/openal-soft-1.21.1/$<CONFIG>/OpenAL32.lib"
    "${CMAKE_SOURCE_DIR}/lib/libsndfile-1.0.31/$<CONFIG>/sndfile.lib"
)

# Make sure the compiler can find include files for Audio library
# when other libraries or executables link to AudioSystem
# And make any files in the src directory accessable as includes
# (none of this #include "../[filename].h" crap)
target_include_directories (AudioSystem PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/inc"
    "${CMAKE_SOURCE_DIR}/src"
)

