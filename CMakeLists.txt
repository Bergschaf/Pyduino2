cmake_minimum_required(VERSION 3.27)
project(Pyduino2)

set(CMAKE_CXX_STANDARD 14)

add_executable(Pyduino2
        riscv_interpreter/main.cpp)
