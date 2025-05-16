#!/usr/bin/env bash
# rm -rf build

cmake -S ./ -B build \
    -G Ninja \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DCMAKE_BUILD_TYPE=DEBUG \
    # -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

cmake --build build -j$(nproc) -v

./build/src/vktt
