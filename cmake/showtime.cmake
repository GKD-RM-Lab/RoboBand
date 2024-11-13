set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/showtime)

include(FetchContent)
FetchContent_Declare(
    showtime
    GIT_REPOSITORY https://github.com/GKD-RM-Lab/showtime.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_DIR ${MOUDLE_DIR}
)
FetchContent_MakeAvailable(showtime)

