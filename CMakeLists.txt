cmake_minimum_required(VERSION 3.30)

project(
    RoboBand 
    VERSION 0.1
    DESCRIPTION "Robot Controller for Robomaster"
    LANGUAGES C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/ext")
include(eigen)
include(tomlplusplus)
include(crcpp)
include(easyloggingpp)
include(imgui)
include(implot)

set(USE_WEBOTS OFF)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(watch)
add_subdirectory(src)
add_subdirectory(app)

