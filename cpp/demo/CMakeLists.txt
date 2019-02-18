add_executable(demo
        main.cpp microchaos.cpp microchaos.h)

target_link_libraries(demo LINK_PUBLIC cm)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(demo PUBLIC OpenMP::OpenMP_CXX)
endif()

