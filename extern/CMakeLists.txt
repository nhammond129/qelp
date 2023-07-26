FetchContent_Declare(
    box2d
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/Box2D"
    GIT_REPOSITORY "https://github.com/erincatto/box2d.git"
    GIT_TAG        "v2.4.1"
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    SFML
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/SFML"
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "2.6.0"
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    ImGui-SFML
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml"
    GIT_REPOSITORY "https://github.com/SFML/imgui-sfml.git"
    GIT_TAG        "c063ec641741f935b914106801e8d607687129dc"
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    imgui
    SOURCE_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG        "v1.89.7"
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    doxygen-awesome-css
    SOURCE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-css"
    GIT_REPOSITORY  "https://github.com/jothepro/doxygen-awesome-css.git"
    GIT_TAG         "v2.2.1"
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)

FetchContent_MakeAvailable(SFML imgui box2d)

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