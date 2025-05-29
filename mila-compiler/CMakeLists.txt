#add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(lexer)

add_executable(mila main.cpp)

# Link
target_link_libraries(mila
  PRIVATE
    parser
    lexer
)

set_target_properties(mila PROPERTIES OUTPUT_NAME mila
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
