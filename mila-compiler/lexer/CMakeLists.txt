set(SOURCE_LIST
include/lexer/lexer.h

src/lexer.cpp
)

add_library(lexer STATIC ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(lexer PUBLIC include)
