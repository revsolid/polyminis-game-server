#!/bin/sh
mkdir ../bin

clang++ -std=c++11 -I../thirdparty/gamelift/include/ -L../thirdparty/gamelift/lib/amazon_linux_x64 -L../thirdparty/dependencies/sio_libs -Wl,-rpath=. -o ../bin/Main ../src/Main.cpp -laws-cpp-sdk-gamelift-server -lsioclient -lboost_date_time -lboost_system

cp ../thirdparty/dependencies/sio_libs/lib*.so* ../bin/
cp ../thirdparty/dependencies/boost_libs/lib*.so* ../bin/
cp ../thirdparty/gamelift/lib/amazon_linux_x64/libaws-cpp-sdk-gamelift-server.so ../bin/
