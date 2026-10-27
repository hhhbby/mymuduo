cmake_minimum_required(VERSION 3.20)

project(mymuduo)
# project(mymuduo LANGUAGES CXX C)

set(SRCLIST 
    TimeStamp.cpp
    InetAddr.cpp
)

add_library(mymuduo SHARED ${SRCLIST})

target_compile_features(mymuduo PUBLIC cxx_std_11)

target_compile_options(mymuduo PRIVATE -g -Wall)

set_target_properties(mymuduo PROPERTIES
    CXX_EXTENSIONS OFF
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
)