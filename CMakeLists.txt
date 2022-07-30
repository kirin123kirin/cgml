cmake_minimum_required(VERSION 3.15)
# set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

project(cgml)

set(REAL_LIBNAME _cgml)

add_executable(cgml src/main.cpp)

