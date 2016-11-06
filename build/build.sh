#!/bin/sh

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
mkdir -p ../binTmp
cd ../binTmp
cmake ../src && make

mkdir -p ../bin

cp ../binTmp/Main ../bin/
cp ../thirdparty/dependencies/sio_libs/lib*.so* ../bin/
cp ../thirdparty/dependencies/boost_libs/lib*.so* ../bin/
cp ../thirdparty/gamelift/lib/amazon_linux_x64/libaws-cpp-sdk-gamelift-server.so ../bin/
