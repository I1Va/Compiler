cmake_minimum_required(VERSION 3.10)
project(stack LANGUAGES C CXX)

add_library(stack STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stack_err_proc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stack_funcs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stack_output.cpp
)

target_include_directories(stack
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(stack
    PUBLIC string_funcs
)

install(TARGETS stack
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/stack
)