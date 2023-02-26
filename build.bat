@echo off

if not exist build (
    mkdir build
)

cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cd ..
cmake --build build -j 8
