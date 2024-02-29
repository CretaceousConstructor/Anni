cmake_minimum_required(VERSION 3.15)
find_package(Vulkan REQUIRED)
if (MSVC)
    add_compile_options(/std:c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS FALSE)
endif()


project(AnniRenderer)
add_definitions(-DNOMINMAX)
add_definitions(-DKHRONOS_STATIC)

#message("The value of my_variable is: ${CMAKE_SOURCE_DIR}")


file(GLOB SourceFiles "AnniRenderer/src/*.cpp")
file(GLOB HeaderFiles "AnniRenderer/*.h")


add_executable(AnniRenderer  ${SourceFiles} ${HeaderFiles})

target_include_directories(AnniRenderer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/AnniRenderer") 
target_link_libraries(AnniRenderer PRIVATE AnniGLTF AnniBase AnniImGui Vulkan::Vulkan glfw ktx stb_image dxc)

