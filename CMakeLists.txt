cmake_minimum_required(VERSION 3.12)
project(main
    VERSION 0.0.1
    LANGUAGES CXX
)

include(FetchContent)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/scenes"
)
add_subdirectory("${CMAKE_SOURCE_DIR}/extern")
add_subdirectory("${CMAKE_SOURCE_DIR}/docs")

add_executable(main)
target_sources(main PRIVATE
    "${SOURCE_DIR}/main.cpp"
    "${SOURCE_DIR}/scenes/MenuScene.cpp"
    "${SOURCE_DIR}/scenes/SceneManager.cpp"
    "${SOURCE_DIR}/util.cpp"
)
target_link_libraries(main PRIVATE
    sfml-graphics
    sfml-network
    sfml-system
    sfml-window
    IMGUI
    ImGui-SFML::ImGui-SFML
    box2d
)

add_executable(phystest)
target_sources(phystest PRIVATE
	"${SOURCE_DIR}/phystest.cpp"
)
target_link_libraries(phystest PRIVATE
	box2d
)

add_executable(server)
target_sources(server PRIVATE
    "${SOURCE_DIR}/server.cpp"
    "${SOURCE_DIR}/util.cpp"
)
target_link_libraries(server PRIVATE
    sfml-graphics
    sfml-network
    sfml-system
    sfml-window
    box2d
)

file(GLOB_RECURSE ASSETS
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_SOURCE_DIR}/assets/"
    "${CMAKE_SOURCE_DIR}/assets/*"
)
foreach(ASSET ${ASSETS})
    add_custom_command(TARGET main PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/assets/${ASSET}"
            "$<TARGET_FILE_DIR:main>/${ASSET}"
    )
endforeach()
