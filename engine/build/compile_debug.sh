#!/bin/sh

find . -mindepth 1 ! \( -name "*.sh" -o -name "*.ps1" \) -delete && cmake -DCMAKE_BUILD_TYPE=Debug .. && make
