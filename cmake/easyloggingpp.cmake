set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/easyloggingpp)

include(FetchContent)
FetchContent_Declare(
    easyloggingpp
    GIT_REPOSITORY https://github.com/abumq/easyloggingpp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    CMAKE_ARGS -Dbuild_static_lib=ON
    SOURCE_DIR ${MOUDLE_DIR}
    SOURCE_SUBDIR /tmp
)
FetchContent_MakeAvailable(easyloggingpp)

add_library(easyloggingpp SHARED ${MOUDLE_DIR}/src/easylogging++.cc)
target_compile_options(easyloggingpp PRIVATE -Wno-all)
target_compile_definitions(
    easyloggingpp 
    PUBLIC ELPP_STL_LOGGING
           ELPP_NO_DEFAULT_LOG_FILE)
