cmake_minimum_required(VERSION 3.10)
project(BackEnd LANGUAGES CXX)

add_library(BackEnd STATIC
    src/ast_translator.cpp
    src/BackEnd.cpp
    src/stack_frame_proc.cpp
)

target_include_directories(BackEnd
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(BackEnd PUBLIC
    FrontEnd
    string_funcs
    graphviz_funcs
    stack
    general_lib
)

install(TARGETS FrontEnd
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/FrontEnd
)
