set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/crcpp)

include(FetchContent)
FetchContent_Declare(
    crcpp
    GIT_REPOSITORY https://github.com/d-bahr/CRCpp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_DIR ${MOUDLE_DIR}
    SOURCE_SUBDIR /tmp
)
FetchContent_MakeAvailable(crcpp)

