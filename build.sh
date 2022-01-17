#!/bin/bash

################################################################################
#                              Usage:                                          #
#  No flags .. Generate CMake files and build                                  #
#  -c ........ Clean build (empty build directory before building)             #
#  -b ........ Build only                                                      #
#  -r ........ Build and run (Equivalent to Visual Studio play button)         #
#  -b-rel .... Build in release mode (Not working yet)                         #
#  -r-rel .... Build and run in release mode (Also not working)                #
#  -v ........ Verbose flag                                                    #
################################################################################

USAGE="Usage: $(basename $0) [ -c | -b | -r | -b-rel | -r-rel ]"
VERBOSE=""

# Empty the build directory
clean () {
    rm -r build/*
}

# Generate the CMake files in Debug
generate () {
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug ..
}

# Build the project in Debug
build () {
    cmake --build build --config Debug -j $VERBOSE
}

# Run the executable
run () {
    pushd build/Debug/
    ./CompetitiveParkingSimulator
    popd
}

# Generate the CMake files in Release
generate_release () {
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release ..
}

# Build the project in Release
build_release () {
    cmake --build build --config Release -j $VERBOSE
}

# Run the executable in Rleease
run_release () {
    pushd build/Release/
    ./CompetitiveParkingSimulator
    popd
}

for arg; do
  case $arg in
    -c) clean && generate && build ; exit 0 ;;
    -b) build ; exit 0 ;;
    -r) build && run ; exit 0 ;;
    -b-rel) build_release ; exit 0 ;;
    -r-rel) build_release && run_release ; exit 0 ;;
    -v) VERBOSE="-v" ;;
     *) echo -e "unrecognized option $arg\n$USAGE" >&2; exit 1 ;;
  esac
done

# Default action
generate && build
