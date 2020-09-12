cmake_minimum_required(VERSION 3.13.4)

project(simpledb)

set(EXEC simpledb)
set(CMAKE_CXX_STANDARD 14)

include_directories(include)
file(GLOB SOURCES src/*.cpp)

add_executable(${EXEC} ${SOURCES})
