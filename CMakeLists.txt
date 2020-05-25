cmake_minimum_required(VERSION 3.16)
project(NES CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(NES
	src/main.cpp 
	src/cpu.h
	src/cpu.cpp
	src/memory.h
    src/cpumemory.h
	src/cpumemory.cpp)

source_group(thing REGULAR_EXPRESSION src/cpu.*)

#target_include_directories(NES PRIVATE include)
