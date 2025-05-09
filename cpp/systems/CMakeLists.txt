add_library(systems
    duffing.cpp
    ikeda.cpp
    jerk_saturated_acc.cpp
    lorenz.cpp
    microchaos.cpp
    pendulum.cpp
    rolling.cpp)

target_include_directories(systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(systems PUBLIC ${CMAKE_SOURCE_DIR}/cm)