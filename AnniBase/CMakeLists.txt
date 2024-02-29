cmake_minimum_required(VERSION 3.15)
find_package(Vulkan REQUIRED)
if (MSVC)
    add_compile_options(/std:c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS FALSE)
endif()



project(AnniBase)
add_definitions(-DNOMINMAX)
add_definitions(-DKHRONOS_STATIC)


set(ProjectType STATIC)

file(GLOB SourceFiles "AnniBase/src/*.cpp")
file(GLOB HeaderFiles "AnniBase/*.h")

add_library(AnniBase ${ProjectType} ${SourceFiles} ${HeaderFiles})

target_include_directories(AnniBase PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/AnniBase") 
target_include_directories(AnniBase INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}") 

target_link_libraries(AnniBase PRIVATE Vulkan::Vulkan AnniGLTF AnniImGui  glm vma stb_image dxc)
target_link_libraries(AnniBase PRIVATE glfw)
target_link_libraries(AnniBase PRIVATE ktx)