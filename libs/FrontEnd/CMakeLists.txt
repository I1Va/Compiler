cmake_minimum_required(VERSION 3.10)
project(FrontEnd LANGUAGES CXX)

add_library(FrontEnd STATIC
    src/AST_io.cpp
    src/AST_proc.cpp
    src/FrontEnd.cpp
    src/lang_grammar.cpp
    src/lang_lexer.cpp
    src/lang_logger.cpp
)

target_include_directories(FrontEnd
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(FrontEnd PUBLIC
    string_funcs
    graphviz_funcs
    general_lib
)

install(TARGETS FrontEnd
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/FrontEnd
)
