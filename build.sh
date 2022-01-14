#!/bin/bash

################################################################################
#                              Usage:                                          #
# No flags . Generate CMake files and build                                    #
# -c ....... Clean build (empty build/ directory before building)              #
# -b ....... Build only (Do not regenerate CMake files)                        #
# -r ....... Build and run (Equivalent to Visual Studio play button)           #
################################################################################

# Empty the build directory
clean () {
    rm -r build/*
}

# Generate the CMake files
generate () {
    pushd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    popd
}

# Build the project (from generated CMake files)
build () {
    pushd build
    cmake --build .
    popd
}

# Run the executable
run() {
    pushd build/Debug
    ./CPSC-585-Project
    popd
}

while getopts 'bcr' clean
do
  case $clean in
    b) build ;;
    c) clean && generate && build ;; 
    r) build && run ;;
  esac
done

# Default: Generate CMake files and build project
# No cleaning or running
generate && build
