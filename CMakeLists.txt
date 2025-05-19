# ───────────────────────────────────────────────────────────
# Корневой CMakeLists.txt
# ───────────────────────────────────────────────────────────

cmake_minimum_required(VERSION 3.10)
project(Compiler VERSION 1.0 LANGUAGES C CXX)

# Output dirs
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подмодули

add_subdirectory(libs/FrontEnd)
# add_subdirectory(libs/MiddleEnd)
add_subdirectory(libs/BackEnd)
add_subdirectory(libs/elf_translator)
add_subdirectory(libs/my_linker)

add_subdirectory(libs/string_funcs)
add_subdirectory(libs/stack)
add_subdirectory(libs/graphviz_funcs)
add_subdirectory(libs/general_lib)


# Главная программа
add_executable(Compiler
    main.cpp
)

target_link_libraries(Compiler PRIVATE
    FrontEnd
    # MiddleEnd
    BackEnd
    my_linker
    elf_translator

    graphviz_funcs
    stack
    string_funcs
    general_lib
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Compiler PRIVATE
        -fsanitize=address
        -fsanitize=alignment
        -fsanitize=bool
        -fsanitize=bounds
        -fsanitize=enum
        -fsanitize=float-cast-overflow
        -fsanitize=float-divide-by-zero
        -fsanitize=integer-divide-by-zero
        -fsanitize=nonnull-attribute
        -fsanitize=null
        -fsanitize=object-size
        -fsanitize=return
        -fsanitize=returns-nonnull-attribute
        -fsanitize=return
        -fsanitize=shift
        -fsanitize=return
        -fsanitize=signed-integer-overflow
        -fsanitize=undefined
        -fsanitize=unreachable
        -fsanitize=vla-bound
        -fsanitize=vptr
    )
    target_link_options(Compiler PRIVATE
    -fsanitize=address
        -fsanitize=alignment
        -fsanitize=bool
        -fsanitize=bounds
        -fsanitize=enum
        -fsanitize=float-cast-overflow
        -fsanitize=float-divide-by-zero
        -fsanitize=integer-divide-by-zero
        -fsanitize=nonnull-attribute
        -fsanitize=null
        -fsanitize=object-size
        -fsanitize=return
        -fsanitize=returns-nonnull-attribute
        -fsanitize=return
        -fsanitize=shift
        -fsanitize=return
        -fsanitize=signed-integer-overflow
        -fsanitize=undefined
        -fsanitize=unreachable
        -fsanitize=vla-bound
        -fsanitize=vptr
    )
endif()
