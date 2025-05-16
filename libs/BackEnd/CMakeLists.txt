cmake_minimum_required(VERSION 3.10)
project(BackEnd LANGUAGES CXX)

add_library(BackEnd STATIC
    src/ast_translator.cpp
    src/back_args_proc.cpp
)

target_include_directories(BackEnd
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(BackEnd PUBLIC
    ASTio
    string_funcs
    graphviz_funcs
    stack
)

install(TARGETS FrontEnd
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/FrontEnd
)
