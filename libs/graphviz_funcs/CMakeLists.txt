cmake_minimum_required(VERSION 3.10)
project(graphviz_funcs LANGUAGES C CXX)

add_library(graphviz_funcs STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dot_log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphviz_funcs.cpp
)

target_include_directories(graphviz_funcs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(graphviz_funcs PUBLIC
    string_funcs
    general_lib
)

install(TARGETS graphviz_funcs
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/graphviz_funcs
)