add_executable(demo-microchaos demo-microchaos.cpp microchaos.cpp microchaos.h)
add_executable(demo-duffing    demo-duffing.cpp duffing.cpp duffing.h)
add_executable(demo-pendulum   demo-pendulum.cpp pendulum.cpp pendulum.h)
add_executable(demo-clustered  demo-clustered.cpp microchaos.cpp microchaos.h)
add_executable(demo-rotation   demo-rotation.cpp)

target_link_libraries(demo-microchaos LINK_PUBLIC cm)
target_link_libraries(demo-duffing    LINK_PUBLIC cm)
target_link_libraries(demo-pendulum   LINK_PUBLIC cm)
target_link_libraries(demo-clustered  LINK_PUBLIC cm)
target_link_libraries(demo-rotation   LINK_PUBLIC cm)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(demo-microchaos PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(demo-duffing    PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(demo-pendulum   PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(demo-clustered  PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(demo-rotation   PUBLIC OpenMP::OpenMP_CXX)
endif()

add_custom_target(all-demos)
add_dependencies(all-demos demo-microchaos demo-duffing demo-pendulum demo-clustered demo-rotation)

