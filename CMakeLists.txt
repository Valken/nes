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

# https://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
# This is the "modern" CMake way. Operating on targets, not setting globals
# I like it.
target_compile_options(NES PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
		)

add_executable(CPU_Test
		test/cpu_tests.cpp
		src/cpu.h
		src/cpu.cpp
		src/memory.h test/cputests.h test/cpu_instruction_load_store.cpp test/cpu_instruction_jump_call.cpp test/cpu_instruction_system.cpp test/cpu_instruction_register_transfers.cpp test/cpu_instruction_arithmetic.cpp test/cpu_instruction_shifts.cpp)

target_compile_options(CPU_Test PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
		)
target_include_directories(CPU_Test PRIVATE ${gtest_SOURCE_DIR}/include)
target_link_libraries(CPU_Test gtest gtest_main)

#source_group(thing REGULAR_EXPRESSION src/cpu.*)

#target_include_#[[]]directories(NES PRIVATE include)

