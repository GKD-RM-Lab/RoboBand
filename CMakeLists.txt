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
include(crcpp)
include(easyloggingpp)

set(USE_WEBOTS OFF)
if(USE_WEBOTS)
    add_definitions(-DUSE_WEBOTS)
    include_directories($ENV{WEBOTS_HOME}/include/controller/cpp)
    link_libraries($ENV{WEBOTS_HOME}/lib/controller/libCppController.so)
endif()

add_subdirectory(src)
add_subdirectory(app)

