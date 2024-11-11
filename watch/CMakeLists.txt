file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.c" "*.cc" "*.cpp")

add_library(watch SHARED ${SRC})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

target_include_directories(watch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(watch PRIVATE cxx_std_23)
target_compile_options(watch PRIVATE -Wall)
target_link_libraries(watch PRIVATE imgui implot)

