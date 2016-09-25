#!/bin/sh
mkdir -p ../bin
cd ../bin
cmake ../src && make

cp ../thirdparty/dependencies/sio_libs/lib*.so* ../bin/
cp ../thirdparty/dependencies/boost_libs/lib*.so* ../bin/
cp ../thirdparty/gamelift/lib/amazon_linux_x64/libaws-cpp-sdk-gamelift-server.so ../bin/
