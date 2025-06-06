cmake_minimum_required(VERSION 3.12)
project(mila)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-gnu-case-range ${CMAKE_CXX_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for IntelliSense

option(BUILD_TESTING ON)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

add_subdirectory(external)
add_subdirectory(mila-compiler)

if (BUILD_TESTING)
  include(CMakeModules/tests.cmake)
endif()
