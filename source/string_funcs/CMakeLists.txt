cmake_minimum_required(VERSION 3.10)
project(string_funcs LANGUAGES C CXX)

add_library(string_funcs STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string_funcs.cpp
)

target_include_directories(string_funcs
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

install(TARGETS string_funcs
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/string_funcs
)