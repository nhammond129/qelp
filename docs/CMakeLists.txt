include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    SOURCE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen-awesome-css"
    GIT_REPOSITORY  "https://github.com/jothepro/doxygen-awesome-css.git"
    GIT_TAG         "a7f7891706c656903326f79baf74beb2b711688d" # v2.2.0
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    FetchContent_MakeAvailable(doxygen-awesome-css)

    set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    add_custom_target(
        docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif(DOXYGEN_FOUND)