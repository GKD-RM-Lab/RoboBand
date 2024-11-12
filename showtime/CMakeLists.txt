file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.c" "*.cc" "*.cpp")

add_library(showtime SHARED ${SRC})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

target_include_directories(showtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(showtime PRIVATE cxx_std_23)
target_compile_options(showtime PRIVATE -Wall)
target_compile_definitions(showtime PUBLIC USE_SHOWTIME)
target_link_libraries(showtime PRIVATE imgui implot tomlplusplus)

