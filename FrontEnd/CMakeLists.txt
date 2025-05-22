cmake_minimum_required(VERSION 3.14)
project(FrontEnd)

include(FetchContent)

FetchContent_Declare(
  utility_lib
  GIT_REPOSITORY git@github.com:I1Va/utility_lib.git
  GIT_TAG        main
  GIT_SHALLOW    FALSE
)
FetchContent_Declare(
  grapher_lib
  GIT_REPOSITORY git@github.com:I1Va/grapher_lib.git
  GIT_TAG        main
  GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(utility_lib)
FetchContent_MakeAvailable(grapher_lib)


add_executable(FrontEnd
    main.cpp
)

target_include_directories(FrontEnd
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(FrontEnd PUBLIC
    utility_lib
    grapher_lib
)

install(TARGETS FrontEnd
    ARCHIVE DESTINATION FrontEnd
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    DESTINATION include/FrontEnd
)
