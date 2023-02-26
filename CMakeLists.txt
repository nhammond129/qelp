cmake_minimum_required(VERSION 3.12)
project(qelp
    VERSION 0.0.1
    LANGUAGES CXX
)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(extern/SFML)

add_executable(qelp)

target_sources(qelp PRIVATE
    "${SOURCE_DIR}/main.cpp"
    "${SOURCE_DIR}/util.cpp"
)

target_include_directories(qelp PUBLIC "${SOURCE_DIR}/include")

target_link_libraries(qelp sfml-graphics sfml-window sfml-system)