cmake_minimum_required(VERSION 3.15)
project(AnniEngine VERSION 1.0.0)
add_definitions(-DNOMINMAX)

if (MSVC)
    add_compile_options(/std:c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS FALSE)
endif()


set (SubProjects
    third_party
    AnniBase
    AnniRenderer
)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


foreach(Project IN LISTS SubProjects)
    if(NOT TARGET Project)  #header guard version
        add_subdirectory(${Project})
    endif()
endforeach()

set_property(TARGET AnniRenderer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")





