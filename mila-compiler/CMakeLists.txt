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
