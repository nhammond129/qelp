FetchContent_Declare(
    SFML
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/SFML"
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "90f05ac" # master (March 12, 2023)
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    ImGui-SFML
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml"
    GIT_REPOSITORY "https://github.com/SFML/imgui-sfml.git"
    GIT_TAG        "f0d8f72" # master (March 12, 2023)
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    imgui
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG        "458a109" # 1.89.3
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    doxygen-awesome-css
    SOURCE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-css"
    GIT_REPOSITORY  "https://github.com/jothepro/doxygen-awesome-css.git"
    GIT_TAG         "a7f7891" # v2.2.0
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)

FetchContent_MakeAvailable(SFML imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
add_library(IMGUI STATIC)
target_sources(IMGUI PRIVATE
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
)


option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
FetchContent_MakeAvailable(ImGui-SFML)