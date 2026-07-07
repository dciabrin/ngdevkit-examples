#!/bin/sh
set -e
cmake -B build-cmake -G Ninja
cmake --build build-cmake
