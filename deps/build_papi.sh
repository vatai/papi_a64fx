#!/usr/bin/bash

[ -e papi ] || git clone https://bitbucket.org/icl/papi.git

# export CC=fcc
# export CXX=FCC

pushd papi

PREFIX=$(pwd)/opt
rm -rf build
mkdir build
cd build
../src/configure --prefix=$PREFIX
make -j
make install

popd
