cmake_minimum_required(VERSION 3.10)
project(elf_translator LANGUAGES CXX)

file(GLOB_RECURSE ElfTranslator_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(elf_translator STATIC ${ElfTranslator_SOURCES})

target_include_directories(elf_translator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

install(TARGETS elf_translator
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/elf_translator
)
