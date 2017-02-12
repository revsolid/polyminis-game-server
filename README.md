# polyminis-game-server
Polyminis Game Server


Game Server. Talks to client directly through websocket and other servers using HTTP.

Note this intended to be deployed inside an Amazon Linux box (what comes with your free tier). 
However with proper setup can build/run in UNIX/LINUX environment (yes your own computer).

To build within UNIX/Linux: 
* You MUST install [CMake](https://cmake.org/install/);
* You MUST install Clang;
* You MUST install Boost C++ Library;

To build:
```
> cd build
> ./build.sh
```
To run:
```
> cd bin
> ./Main
```
Ping Roy if things don't work or if you have problem setting up.