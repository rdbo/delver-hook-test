cmake_minimum_required(VERSION 3.21)

project(delver-hook LANGUAGES C CXX ASM)

find_package(JNI REQUIRED)

add_subdirectory(external/jnihook)

add_library(delverhook SHARED src/main.cpp)
target_link_libraries(delverhook PRIVATE jnihook ${JNI_LIBRARIES})
target_include_directories(delverhook PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/jnihook/include
)
set_target_properties(delverhook PROPERTIES POSITION_INDEPENDENT_CODE True)
