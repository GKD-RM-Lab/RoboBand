file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.c" "*.cc" "*.cpp")

add_library(src STATIC ${SRC})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(src PRIVATE cxx_std_23)
target_compile_options(src PRIVATE -Wall)
target_link_libraries(src PUBLIC 
    c
    m 
    dl 
    rt 
    pthread
    easyloggingpp
    tomlplusplus)

if(USE_WEBOTS)
    include(webots)
    target_link_libraries(src PUBLIC webots)
endif()

