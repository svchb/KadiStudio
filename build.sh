#!/bin/sh

BUILDFOLDER=build
CMAKE_BUILD_TYPE=Release
# CMAKE_BUILD_TYPE=Debug

cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -S . -B $BUILDFOLDER
cmake --build $BUILDFOLDER --parallel $(nproc --all)
