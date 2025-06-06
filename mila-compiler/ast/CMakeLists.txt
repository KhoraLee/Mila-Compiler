set(SOURCE_LIST
include/ast/node.h
include/ast/declaration.h
include/ast/expression.h
include/ast/statement.h

src/node.cpp
)

add_library(ast STATIC ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(ast PUBLIC include)
