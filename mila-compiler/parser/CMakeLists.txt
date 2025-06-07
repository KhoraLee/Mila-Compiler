set(SOURCE_LIST
include/parser/parser.h

src/parser.cpp
src/decl_parser.cpp
src/expr_parser.cpp
src/stmt_parser.cpp
)

add_library(parser STATIC ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(parser PUBLIC include)
target_link_libraries(parser PUBLIC ast exception)
