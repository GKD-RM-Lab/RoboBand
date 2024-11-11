set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/implot)

include(FetchContent)
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_DIR ${MOUDLE_DIR}
    SOURCE_SUBDIR /tmp
)
FetchContent_MakeAvailable(implot)

add_library(implot SHARED ${MOUDLE_DIR}/implot.cpp ${MOUDLE_DIR}/implot_items.cpp)
target_include_directories(implot PUBLIC ${MOUDLE_DIR})
target_link_libraries(implot PUBLIC imgui)

