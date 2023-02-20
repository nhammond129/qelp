cmake_minimum_required(VERSION 3.12)
project("qelp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(SOURCE_FILES
    src/main.cpp
)

add_subdirectory(extern/SFML)

add_executable(qelp ${SOURCE_FILES})

target_link_libraries(qelp sfml-graphics sfml-window sfml-system)