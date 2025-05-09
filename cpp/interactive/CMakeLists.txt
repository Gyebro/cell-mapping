find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(interactive_ui_SOURCES
        executor.cpp
        cscm-frame.cpp
        interactive-cscm.cpp
)
set(interactive_ui_HEADERS
        cscm-frame.h
)
qt5_wrap_cpp(interactive_MOCS ${interactive_ui_HEADERS})
add_executable(interactive-cscm ${interactive_ui_SOURCES} ${interactive_MOCS})
target_link_libraries(interactive-cscm Qt5::Widgets systems cm)

find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    message(INFO " accelerating with OpenMP")
    target_link_libraries(interactive-cscm OpenMP::OpenMP_CXX)
endif()