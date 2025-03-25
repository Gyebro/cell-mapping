cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 14)

option(QT_SUPPORT "Build with Qt interface support" ON)

if(QT_SUPPORT)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
endif()

add_library(cm
		cmlib.h
		css.cpp css.h
		cell.cpp cell.h
		scm.cpp scm.h
		scmb.cpp scmb.h
		system.cpp system.h
		vec2.cpp vec2.h
		vec3.cpp vec3.h
		rk45.cpp rk45.h
		coloring.cpp coloring.h
		cscm.cpp cscm.h)

target_include_directories(cm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(QT_SUPPORT)
	target_link_libraries(cm jpeg Qt5::Widgets)
else()
	target_link_libraries(cm jpeg)
endif()