cmake_minimum_required(VERSION 3.10)
project(ASTio LANGUAGES C CXX)

file(GLOB_RECURSE ASTio_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(ASTio STATIC ${ASTio_SOURCES})

target_include_directories(ASTio
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(ASTio
    PUBLIC BackEnd
)

install(TARGETS ASTio
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/ASTio
)