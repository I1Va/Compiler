cmake_minimum_required(VERSION 3.10)
project(general_lib LANGUAGES C CXX)

add_library(general_lib STATIC
    src/general.cpp
)

target_include_directories(general_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

install(TARGETS general_lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/general_lib
)