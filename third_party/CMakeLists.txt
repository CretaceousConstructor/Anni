cmake_minimum_required(VERSION 3.15)
find_package(Vulkan REQUIRED) #if not, use header and .lib way of importing
if (MSVC)
    add_compile_options(/std:c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()


#HEADER ONLY LIBRARYS
#vma glm, stb_image are header only libs so we only need the include path
add_library(glm INTERFACE)
add_library(vma INTERFACE)
add_library(stb_image INTERFACE)

target_include_directories(glm INTERFACE glm)
target_include_directories(vma INTERFACE vma)
target_include_directories(stb_image INTERFACE stb_image)


#GLFW IS A lIB WITH ONLY .LIB AND .H FILES
add_library(glfw INTERFACE)
target_include_directories(glfw INTERFACE glfw/include)
target_link_libraries(glfw INTERFACE glfw/lib)
##on windows
#target_link_libraries(someShitObject PRIVATE glfw/lib/glfw3.lib)


add_library(ktx INTERFACE)
target_include_directories(ktx INTERFACE ktx/include)
target_link_libraries(ktx INTERFACE ktx/lib)




#COMPILE FROM SOURCE CODE, AND SUB-DIR ALSO HAVE Cmakelists.txt FILE
add_subdirectory(fastgltf)
add_library(AnniGLTF INTERFACE)
target_link_libraries(AnniGLTF INTERFACE fastgltf)





#COMPILE FROM BARE SOURCE CODE
add_library(AnniImGui STATIC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_vulkan.cpp

        imgui/imgui.h
        imgui/imgui_impl_glfw.h
        imgui/imgui_impl_vulkan.h
)

target_include_directories(AnniImGui PUBLIC imgui)
target_link_libraries(AnniImGui PRIVATE glfw)
target_link_libraries(AnniImGui PRIVATE Vulkan::Vulkan)





