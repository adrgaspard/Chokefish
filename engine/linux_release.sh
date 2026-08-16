#!/bin/sh

set -e

cd "$(dirname "$0")"

rm -rf build-linux
cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux -j$(nproc)
