cmake_minimum_required(VERSION 3.12)
project(qelp)
add_subdirectory(extern/SFML)

add_executable(binary "")
set_target_properties(binary PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    BUILD_SHARED_LIBS OFF
)

target_sources(binary PRIVATE
    src/main.cpp
)

target_link_libraries(binary PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

add_custom_target(run(
    COMMAND ${PROJECT_BINARY_DIR}/build/bin/binary.exe
    DEPENDS binary
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
))