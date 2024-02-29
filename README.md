Anni(stands for "annihilation") is a graphics engine implemented using vulkan graphics api and c++ 20.

There are several externl librarys on which Anni relys:\
vulkan-hpp: CPP binding of vulkan.\
fastgltf : For fast parallel loading of gltf model file.\
glfw     : Windows system integration.\
glm      : A math library.\
imgui    : For gui used in the engine.\
ktx      : For loading ktx images.\
std_image: For loading stb images.\
vma      : GPU memory allocation.
 
Engine-level features under development:\
RenderGraph:\
    1.Automatic resources lifetime management.\
    2.Automatic resources barriers insertion.\
    3.Automatic descriptor set layouts generation and descriptors allocation.\
    4.Multi-frame inflight syncronization.\
    5.Multi-queue execution scheduling(make full use of available queues) enables async load and async compute.\
Graphics-level features under development:\
    1.MSAA Defered Rendering.\
    2.Physically based rendering.