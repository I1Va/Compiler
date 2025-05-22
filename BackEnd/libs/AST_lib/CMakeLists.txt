cmake_minimum_required(VERSION 3.10)
project(AST_lib LANGUAGES C CXX)

add_library(AST_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AST_io.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AST_proc.cpp
)

target_include_directories(AST_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(AST_lib PUBLIC
    string_funcs
    graphviz_funcs
    general_lib
)

install(TARGETS AST_lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/AST_lib
)