cmake_minimum_required(VERSION 3.16)
project(NES CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/gtest)

add_executable(NES
	src/main.cpp 
	src/cpu.h
	src/cpu.cpp
	src/memory.h
    src/cpumemory.h
	src/cpumemory.cpp)

target_include_directories(NES PRIVATE gtest/googletest/include)
target_link_libraries(NES gtest gtest_main)

#source_group(thing REGULAR_EXPRESSION src/cpu.*)

#target_include_#[[]]directories(NES PRIVATE include)

