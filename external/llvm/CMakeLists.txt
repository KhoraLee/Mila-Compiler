if(APPLE)
	execute_process(
		COMMAND brew --prefix llvm@19
		RESULT_VARIABLE BREW_RESULT
		OUTPUT_VARIABLE BREW_LLVM
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
  set(LLVM_DIR "${BREW_LLVM}/lib/cmake/llvm")
endif()

find_package(LLVM 19.1 CONFIG)
if(NOT LLVM_FOUND)
  message(FATAL_ERROR "LLVM 19.1 not found.")
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
