cmake_minimum_required(VERSION 3.10)
project(ASTio LANGUAGES CXX)

add_library(ASTio STATIC
    src/AST_io.cpp
    src/AST_proc.cpp
)

target_include_directories(ASTio
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(ASTio PUBLIC
    string_funcs
    graphviz_funcs
    general_lib
)

install(TARGETS ASTio
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/ASTio
)
