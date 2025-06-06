set(SOURCE_LIST
include/ast/node.h
include/ast/declaration.h
include/ast/expression.h
include/ast/statement.h
)

add_library(ast INTERFACE ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(ast INTERFACE include)

target_link_libraries(ast INTERFACE lexer)
