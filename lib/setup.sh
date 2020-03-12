#!/bin/bash

GBENCH_URL=https://github.com/google/benchmark.git
GBENCH_DIR=benchmark/
STB_URL=https://github.com/nothings/stb.git  

INITIAL_DIR=$PWD

# Gbench
git clone $GBENCH_URL $GBENCH_DIR
cd $GBENCH_DIR
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON ../
make -j4

# STB
cd $INITIAL_DIR
git clone $STB_URL $STB_DIR
