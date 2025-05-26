cmake_minimum_required(VERSION 3.10)

project(cell-mapping-cpp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libjpeg)
add_subdirectory(cm)
add_subdirectory(systems)
add_subdirectory(gnuplot_i)

add_subdirectory(demo)
add_subdirectory(interactive)