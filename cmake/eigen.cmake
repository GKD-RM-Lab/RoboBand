set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/eigen)

include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_DIR ${MOUDLE_DIR}
)
FetchContent_MakeAvailable(eigen)

