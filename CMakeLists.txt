cmake_minimum_required(VERSION 3.15)
project(GST C)

set(CMAKE_C_STANDARD 11)

add_executable(generator generator.c utils.h utils.c)
add_executable(sequential_execution sequential_execution.c utils.h utils.c)
add_executable(sequential_execution_cp sequential_execution_cp.c utils.h utils.c)
add_executable(openmp openmp.c utils.h utils.c)
add_executable(openmpi openmpi.c utils.h utils.c)
