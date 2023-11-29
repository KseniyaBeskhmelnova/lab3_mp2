cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME lab3)
project(${PROJECT_NAME})
include_directories(include)

add_executable(lab3 src/lab3.cpp include/lab3.hpp)


