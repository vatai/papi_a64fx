#!/usr/bin/bash

[ -e papi ] || git clone https://bitbucket.org/icl/papi.git

# export CC=fcc
# export CXX=FCC

pushd papi

PREFIX=$(pwd)/opt
git checkout papi-7-0-1-t
./src/configure --prefix=$PREFIX
make -j
make install

popd
