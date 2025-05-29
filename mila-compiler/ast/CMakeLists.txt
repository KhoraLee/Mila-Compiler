set(SOURCE_LIST
#include/ast/.h

#src/.cpp
)

add_library(ast STATIC ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(ast PUBLIC include)
#target_link_libraries(cpu PUBLIC)
