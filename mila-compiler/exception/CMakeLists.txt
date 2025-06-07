set(SOURCE_LIST
include/exception/exception.h
include/exception/lexer_exception.h
)

add_library(exception INTERFACE ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(exception INTERFACE include)

target_link_libraries(exception INTERFACE lexer)
