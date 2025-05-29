cmake_minimum_required(VERSION 3.12)
project(mila)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic ${CMAKE_CXX_FLAGS}")

option(BUILD_TESTING ON)

add_subdirectory(external)
add_subdirectory(mila-compiler)

if (BUILD_TESTING)
	include(CMakeModules/tests.cmake)
endif()
