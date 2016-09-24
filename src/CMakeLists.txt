CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (Polyminis)
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/../thirdparty/gamelift/include")
LINK_DIRECTORIES("${PROJECT_SOURCE_DIR}/../thirdparty/gamelift/lib/amazon_linux_x64")


SET(COMPILE_FLAGS "-std=c++11")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}" )

ADD_EXECUTABLE(Main Main.cpp)
TARGET_LINK_LIBRARIES(Main aws-cpp-sdk-gamelift-server) 
