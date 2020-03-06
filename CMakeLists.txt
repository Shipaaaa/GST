cmake_minimum_required(VERSION 3.15)
project(GST C)

set(CMAKE_C_STANDARD 11)

add_executable(generator generator.c)
add_executable(sequential_execution sequential_execution.c)
