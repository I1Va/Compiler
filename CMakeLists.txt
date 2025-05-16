# ───────────────────────────────────────────────────────────
# Корневой CMakeLists.txt
# ───────────────────────────────────────────────────────────

cmake_minimum_required(VERSION 3.10)
project(Compiler VERSION 1.0 LANGUAGES C CXX)

# Output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подмодули

# add_subdirectory(source/FrontEnd)
# add_subdirectory(source/MiddleEnd)
# add_subdirectory(source/BackEnd)
add_subdirectory(source/elf_translator)
add_subdirectory(source/my_linker)

# add_subdirectory(source/ASTio)

add_subdirectory(source/string_funcs)
add_subdirectory(source/stack)
add_subdirectory(source/graphviz_funcs)


# Главная программа
add_executable(Compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(Compiler PRIVATE
    # FrontEnd
    # MiddleEnd
    # BackEnd
    my_linker
    elf_translator
    # ASTio

    graphviz_funcs
    stack
    string_funcs
)

install(TARGETS Compiler DESTINATION bin)
