#!/bin/sh

find . -mindepth 1 ! -name "*.sh" -delete && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 .. && make
