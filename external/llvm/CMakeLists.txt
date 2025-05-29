# taken from https://github.com/RPCS3/rpcs3/blob/master/3rdparty/llvm/CMakeLists.txt
if(BUILD_LLVM)
  message(STATUS "LLVM will be built from the submodule.")

  set(LLVM_TARGETS_TO_BUILD "AArch64;X86" CACHE STRING "Semicolon-separated list of targets to build, or \"all\".")
  option(LLVM_BUILD_RUNTIME "Build the LLVM runtime libraries." OFF)
  option(LLVM_BUILD_TOOLS "Build the LLVM tools. If OFF, just generate build targets." OFF)
  option(LLVM_INCLUDE_BENCHMARKS "Generate benchmark targets. If OFF, benchmarks can't be built." OFF)
  option(LLVM_INCLUDE_DOCS "Generate build targets for llvm documentation." OFF)
  option(LLVM_INCLUDE_EXAMPLES "Generate build targets for the LLVM examples" OFF)
  option(LLVM_INCLUDE_TESTS "Generate build targets for the LLVM unit tests." OFF)
  option(LLVM_INCLUDE_TOOLS "Generate build targets for the LLVM tools." OFF)
  option(LLVM_INCLUDE_UTILS "Generate build targets for the LLVM utils." OFF)
  option(LLVM_CCACHE_BUILD "Set to ON for a ccache enabled build" OFF)
  set(LLVM_ENABLE_WARNINGS OFF CACHE BOOL "Enable compiler warnings.")

  # For Windows x86 (not Windows AArch64).
  # Check on MSVC is needed due to COMPILER_X86, COMPILER_ARM etc. are not set/supported by the MSVC compiler, if used.
  # Furthermore, the MSVC compiler is not available/supported on Windows AArch64
  if(WIN32 AND (COMPILER_X86 OR MSVC))
    set(LLVM_USE_INTEL_JITEVENTS ON)
  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(COMPILER_X86)
      set(LLVM_USE_INTEL_JITEVENTS ON)
    endif()
    set(LLVM_USE_PERF ON)
  endif()

  if (MSVC)
    add_compile_definitions("$<$<COMPILE_LANGUAGE:CXX>:_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS>")
  endif()

  # LLVM needs to be built out-of-tree
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/llvm-project/llvm ${CMAKE_CURRENT_BINARY_DIR}/llvm_build EXCLUDE_FROM_ALL)
  set(LLVM_DIR "${CMAKE_CURRENT_BINARY_DIR}/llvm_build/lib/cmake/llvm/")

  find_package(LLVM 19.1 CONFIG)
  if(NOT LLVM_FOUND)
    message(FATAL_ERROR "Couldn't build LLVM from the submodule. You might need to run `git submodule update --init`")
  endif()
else()
  message(STATUS "Using prebuilt or system LLVM")

  # Locate llvm 19 directories from homebrew
  if(APPLE)
    execute_process(
      COMMAND brew --prefix llvm@19
      RESULT_VARIABLE BREW_RESULT
      OUTPUT_VARIABLE BREW_LLVM
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(BREW_RESULT EQUAL 0)
      # set(LLVM_DIR "${BREW_LLVM}/lib/cmake/llvm")
      list(APPEND CMAKE_PREFIX_PATH "${BREW_LLVM}")
    endif()
  endif()

  find_package(LLVM 19.1 CONFIG)
  if(NOT LLVM_FOUND)
    message(FATAL_ERROR "LLVM 19.1 not found. Enable BUILD_LLVM option to build LLVM from included as a git submodule.")
  endif()
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

llvm_map_components_to_libnames(llvm_libs support core irreader)
message(STATUS "LLVM linking targets: ${llvm_libs}")

add_library(llvm INTERFACE)
target_link_libraries(llvm INTERFACE ${llvm_libs})
target_include_directories(llvm SYSTEM INTERFACE ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
target_compile_definitions(llvm INTERFACE ${LLVM_DEFINITIONS_LIST} LLVM_AVAILABLE)
