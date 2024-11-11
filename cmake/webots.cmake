add_library(webots INTERFACE)
target_include_directories(webots INTERFACE $ENV{WEBOTS_HOME}/include/controller/cpp)
target_compile_definitions(webots INTERFACE USE_WEBOTS)
target_link_libraries(webots INTERFACE $ENV{WEBOTS_HOME}/lib/controller/libCppController.so)
