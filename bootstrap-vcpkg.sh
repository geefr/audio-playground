#!/usr/bin/env sh
./vcpkg/bootstrap-vcpkg.sh -disableMetrics
./vcpkg/vcpkg install

rm -rf build
mkdir build
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_INSTALL_PREFIX=../install ../
