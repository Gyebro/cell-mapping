cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 14)

add_library(cm
		cmlib.h
		css.cpp
		css.h cell.cpp cell.h scm.cpp scm.h system.cpp system.h vec2.cpp vec2.h rk45.cpp rk45.h coloring.cpp coloring.h cscm.cpp cscm.h)

target_include_directories(cm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(cm jpeg)