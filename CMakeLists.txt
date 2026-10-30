cmake_minimum_required(VERSION 3.20)

project(mymuduo)
# project(mymuduo LANGUAGES CXX C)

set(SRCLIST 
    TimeStamp.cpp
    InetAddr.cpp
    Logger.cpp

    Channel.cpp
    EventLoop.cpp
    EpollPoller.cpp
    DefaultPoller.cpp

    Thread.cpp
    CurrentThread.cpp
)

add_library(mymuduo SHARED ${SRCLIST})

target_compile_features(mymuduo PUBLIC cxx_std_11)

set_target_properties(mymuduo PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

target_compile_options(mymuduo PRIVATE -g -Wall)

set_target_properties(mymuduo PROPERTIES
    CXX_EXTENSIONS OFF
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
)