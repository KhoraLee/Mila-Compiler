set(SOURCE_LIST
include/codegen/code_generator.h

src/code_generator.cpp
src/decl_codegen.cpp
src/expr_codegen.cpp
src/stmt_codegen.cpp
)

add_library(code_gen STATIC ${SOURCE_LIST})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

target_include_directories(code_gen PUBLIC include)

target_link_libraries(code_gen PUBLIC parser llvm)
