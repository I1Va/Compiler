# ───────────────────────────────────────────────────────────
# source/my_linker/CMakeLists.txt
# ───────────────────────────────────────────────────────────
cmake_minimum_required(VERSION 3.10)
project(my_linker LANGUAGES CXX)

add_library(my_linker STATIC
    src/compiler_artifacts_proc.cpp)

target_include_directories(my_linker
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

install(TARGETS my_linker
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/my_linker
)
