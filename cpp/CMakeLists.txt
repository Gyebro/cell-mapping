cmake_minimum_required(VERSION 3.9)

project(cell-mapping-cpp)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(libjpeg)
add_subdirectory(cm)

add_subdirectory(demo)