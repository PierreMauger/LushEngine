#!/bin/sh

if [ ! -d "build" ]; then
    mkdir build
fi

cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cd ..

cmake --build build -j 8
