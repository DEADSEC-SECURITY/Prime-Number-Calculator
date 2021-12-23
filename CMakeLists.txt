cmake_minimum_required(VERSION 3.21)
project(PrimeNumbers)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)
set(MALLOC_CHECK_ 2)

add_executable(PrimeNumbers main.cpp)
