#!/bin/bash
rm -rf CMakeCache.txt CMakeFiles/ Makefile cmake_install.cmake
cmake .. -DCMAKE_BUILD_TYPE=Debug
