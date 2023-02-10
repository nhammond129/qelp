cmake_minimum_required(VERSION 3.10)
project("test")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_FILES
    src/main.cpp
)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(deps/SFML)


add_executable(test ${SOURCE_FILES})

target_link_libraries(test sfml-graphics sfml-window sfml-system)