include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
set(CMAKE_FOLDER externals)

# LLVM
add_subdirectory(llvm EXCLUDE_FROM_ALL)
