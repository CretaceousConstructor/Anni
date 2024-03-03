Anni(stands for "annihilation") is a graphics engine implemented using vulkan graphics api and c++ 20.

There are several externl librarys on which Anni relys:\
&ensp; vulkan-hpp: CPP binding of vulkan.\
&ensp; fastgltf : For fast parallel loading of gltf model files.\
&ensp; glfw     : Windows system integration.\
&ensp; glm      : A commonly used math library.\
&ensp; imgui    : For GUI used in the engine.\
&ensp; ktx      : For loading ktx images.\
&ensp; std_image: For loading stb images.\
&ensp; vma      : GPU memory allocation.
 
Engine-level features under development:\
&ensp;   RenderGraphV1:\
&ensp;&ensp;  1.Automatic resources lifetime management.\
&ensp;&ensp;  2.Automatic resources barriers and semaphores insertion.\
&ensp;&ensp;  3.Automatic descriptor set layouts generation and descriptors allocation.\
&ensp;&ensp;   4.Multi-frame inflight syncronization.\
&ensp;&ensp;   5.Multi-queue execution scheduling(make full use of available queues) enables async load and async compute.\
&ensp;   RenderGraphV2:\
&ensp;&ensp;   1.Resources creation decriptor automatic generation(from common image view formats gathered from all image usages specific to  render passes).\
&ensp;&ensp;   2.Remove **format** field of AttachUsage and TexUsage, it can be infered from image view formats by finding the common one.\
Graphics-level features under development:\
&ensp;&ensp;   1.MSAA Defered Rendering.\
&ensp;&ensp;   2.Physically based rendering.


TODO: when model is being imported into RG, better naming scheme is needed.

