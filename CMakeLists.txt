cmake_minimum_required(VERSION 3.21)

project(delver-hook LANGUAGES C CXX ASM)

add_subdirectory(external/jnihook)

add_library(delverhook SHARED src/main.cpp)
target_link_libraries(delverhook PRIVATE jnihook -static-libgcc -static-libstdc++)
set_target_properties(delverhook PROPERTIES POSITION_INDEPENDENT_CODE True)
