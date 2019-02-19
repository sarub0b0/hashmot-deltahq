#!/bin/bash
rm -rf CMakeCache.txt CMakeFiles/ Makefile cmake_install.cmake

if [ -z "$1" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

if [ "$1" = "d" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    exit
fi
if [ "$1" = "r" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi
