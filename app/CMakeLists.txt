file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.c" "*.cc" "*.cpp")

add_executable(${PROJECT_NAME} ${SRC})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} PRIVATE src)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_custom_command(TARGET ${PROJECT_NAME}
    COMMAND sudo setcap CAP_SYS_NICE+eip ${CMAKE_SOURCE_DIR}/build/app/${PROJECT_NAME})

