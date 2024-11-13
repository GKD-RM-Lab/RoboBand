set (MOUDLE_DIR ${CMAKE_SOURCE_DIR}/ext/imgui)

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_DIR ${MOUDLE_DIR}
    SOURCE_SUBDIR /tmp
)
FetchContent_MakeAvailable(imgui)

file(GLOB SRC CONFIGURE_DEPENDS ${MOUDLE_DIR}/*.cpp)

add_library(imgui SHARED
    ${SRC}
    ${MOUDLE_DIR}/backends/imgui_impl_glfw.cpp
    ${MOUDLE_DIR}/backends/imgui_impl_opengl3.cpp
    ${MOUDLE_DIR}/misc/freetype/imgui_freetype.cpp)
target_include_directories(imgui PUBLIC ${MOUDLE_DIR})
target_include_directories(imgui PRIVATE 
    ${MOUDLE_DIR}/backends
    ${MOUDLE_DIR}/misc/freetype
    /usr/include/freetype2)
target_compile_definitions(imgui PUBLIC 
    IMGUI_ENABLE_FREETYPE
    ImDrawIdx=unsigned\ int)
target_link_libraries(imgui PRIVATE GL glfw freetype)

