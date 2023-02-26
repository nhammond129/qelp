cmake_minimum_required(VERSION 3.12)
project(qelp
    VERSION 0.0.1
    LANGUAGES CXX
)


include(FetchContent)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory("${CMAKE_SOURCE_DIR}/extern")
add_executable(qelp)

target_sources(qelp PRIVATE
    "${SOURCE_DIR}/main.cpp"
    "${SOURCE_DIR}/util.cpp"
)

target_include_directories(qelp PUBLIC "${SOURCE_DIR}/include")

target_link_libraries(qelp PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    IMGUI
    ImGui-SFML::ImGui-SFML
)