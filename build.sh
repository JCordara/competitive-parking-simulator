#!/bin/bash

################################################################################
#                              Usage:                                          #
#  No flags .. Configure CMake and build                                       #
#  -c ........ Clean configure and build (empty build directory first)         #
#  -b ........ Build only                                                      #
#  -r ........ Build and run (Equivalent to Visual Studio play button)         #
#  -c-rel .... Clean configure and build in release mode                       #
#  -b-rel .... Build in release mode                                           #
#  -r-rel .... Build and run in release mode                                   #
#  -v ........ Verbose flag                                                    #
################################################################################

USAGE="Usage: $(basename $0) [ -c | -b | -r | -c-rel | -b-rel | -r-rel ]"
VERBOSE=""

# Empty the build directory
clean () {
    rm -r build/*
}

# Generate the CMake files in Debug
generate () {
    cmake -S . -B build ..
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
    cmake -S . -B build ..
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
    -c-rel) clean && generate_release && build_release ; exit 0 ;;
    -b-rel) build_release ; exit 0 ;;
    -r-rel) build_release && run_release ; exit 0 ;;
    -v) VERBOSE="-v" ;;
     *) echo -e "unrecognized option $arg\n$USAGE" >&2; exit 1 ;;
  esac
done

# Default action
generate && build
