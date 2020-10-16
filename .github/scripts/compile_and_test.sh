#!/bin/bash

cd /Package
source init.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_CXX_STANDARD=17   .. && \
make  -j `getconf _NPROCESSORS_ONLN` && \
make install && \
ls -alhR $G4LEDATA && \
ctest -j `getconf _NPROCESSORS_ONLN` --output-on-failure || true
