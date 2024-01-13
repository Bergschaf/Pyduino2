cmake_minimum_required(VERSION 3.10)
project(Serial)
find_package(CppLinuxSerial REQUIRED)
add_executable(Serial Serial_PC.cpp
)
target_link_libraries(Serial CppLinuxSerial::CppLinuxSerial)