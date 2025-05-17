cmake_minimum_required(VERSION 3.10)
project(elf_translator LANGUAGES CXX)

add_library(elf_translator STATIC
    src/elf_gen.cpp
    src/elf_header_gen.cpp
    src/error_proc.cpp
    src/segments_gen.cpp
)

target_include_directories(elf_translator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(elf_translator PUBLIC
    string_funcs
)

install(TARGETS elf_translator
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/elf_translator
)
