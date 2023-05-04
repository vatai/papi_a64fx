#!/usr/bin/bash

[ -e papi ] || git clone https://bitbucket.org/icl/papi.git

# export CC=fcc
# export CXX=FCC

pushd papi

PREFIX=$(pwd)/opt
git checkout papi-7-0-1-t

pushd src

./configure --prefix=$PREFIX
make -j
make install

popd

popd
