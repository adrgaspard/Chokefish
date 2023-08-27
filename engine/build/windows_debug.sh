#!/bin/sh

find . -mindepth 1 ! -name "*.sh" -delete && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 .. && make
