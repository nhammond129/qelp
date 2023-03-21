FetchContent_Declare(
    Box2D
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/Box2D"
    GIT_REPOSITORY "https://github.com/erincatto/box2d.git"
    GIT_TAG        "9ebbbcd960ad424e03e5de6e66a40764c16f51bc" # v2.4.1
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    SFML
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/SFML"
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "641cf54cc34a91ebf57d43b2400ac6436c47b313" # master (March 15, 2023)
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    ImGui-SFML
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml"
    GIT_REPOSITORY "https://github.com/SFML/imgui-sfml.git"
    GIT_TAG        "f0d8f72f073b367e9506b8794158cc91f015252e" # master (March 12, 2023)
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    imgui
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG        "f3f6295d532b98e322691afb8ef418209d963786" # 1.89.4
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    doxygen-awesome-css
    SOURCE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-css"
    GIT_REPOSITORY  "https://github.com/jothepro/doxygen-awesome-css.git"
    GIT_TAG         "52dbc3c5aad0ddcd17bfe924e47937d8972d7699" # v2.2.0 w/ typo fixes
    GIT_PROGRESS    TRUE
)

FetchContent_MakeAvailable(SFML imgui Box2D)

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