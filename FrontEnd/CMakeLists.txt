cmake_minimum_required(VERSION 3.10)
project(FrontEnd LANGUAGES C CXX)

# Output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Submodules
add_subdirectory(libs/string_funcs)
add_subdirectory(libs/graphviz_funcs)
add_subdirectory(libs/general_lib)
add_subdirectory(libs/AST_lib)

# Главная программа
add_executable(FrontEnd
    src/FrontEnd.cpp
    src/lang_grammar.cpp
    src/lang_lexer.cpp
    src/lang_logger.cpp
    src/front_args_proc.cpp
    main.cpp
)

target_include_directories(FrontEnd
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(FrontEnd PRIVATE
    graphviz_funcs
    string_funcs
    general_lib
    AST_lib
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(FrontEnd PRIVATE
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
        -D_DEBUG
        -ggdb3
        -std=c++17
        -O0
        -Wall
        -Wextra
        -Weffc++
        -Waggressive-loop-optimizations
        -Wc++14-compat
        -Wmissing-declarations
        -Wcast-align
        -Wcast-qual
        -Wchar-subscripts
        -Wconditionally-supported
        -Wconversion
        -Wctor-dtor-privacy
        -Wempty-body
        -Wfloat-equal
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-signedness
        -Wformat=2
        -Winline
        -Wlogical-op
        -Wnon-virtual-dtor
        -Wopenmp-simd
        -Woverloaded-virtual
        -Wpacked
        -Wpointer-arith
        -Winit-self
        -Wredundant-decls
        -Wshadow
        -Wsign-conversion
        -Wsign-promo
        -Wstrict-null-sentinel
        -Wstrict-overflow=2
        -Wsuggest-attribute=noreturn
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wsuggest-override
        -Wswitch-default
        -Wswitch-enum
        -Wsync-nand
        -Wundef
        -Wunreachable-code
        -Wunused
        -Wuseless-cast
        -Wvariadic-macros
        -Wno-literal-suffix
        -Wno-missing-field-initializers
        -Wno-narrowing
        -Wno-old-style-cast
        -Wno-varargs
        -Wstack-protector
        -fcheck-new
        -fsized-deallocation
        -fstack-protector
        -fstrict-overflow
        -flto-odr-type-merging
        -fno-omit-frame-pointer
        -Wlarger-than=8192
        -Wstack-usage=8192
        -pie
        -fPIE
        -Werror=vla
    )
    target_link_options(FrontEnd PRIVATE
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
