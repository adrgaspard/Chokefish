#!/bin/sh

find . -mindepth 1 ! -name "*.sh" -delete && cmake -DCMAKE_BUILD_TYPE=Debug .. && make
