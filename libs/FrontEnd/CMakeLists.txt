cmake_minimum_required(VERSION 3.10)
project(FrontEnd LANGUAGES CXX)

add_library(FrontEnd STATIC
    src/lang_global_space.cpp
    src/lang_grammar.cpp
    src/lang_lexer.cpp
    src/lang_logger.cpp
)

target_include_directories(FrontEnd
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(FrontEnd PUBLIC
    ASTio
    string_funcs
    graphviz_funcs
)

install(TARGETS FrontEnd
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/FrontEnd
)
