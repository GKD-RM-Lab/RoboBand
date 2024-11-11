set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/tomlplusplus)

include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_DIR ${MOUDLE_DIR}
)
FetchContent_MakeAvailable(tomlplusplus)

