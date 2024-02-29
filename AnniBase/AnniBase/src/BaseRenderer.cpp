#include "BaseRenderer.h"


namespace Anni::Renderer
{
	IRenderer::IRenderer(GraphicsComponent& gfx_)
		:
		gfx(gfx_),
		device_manager(gfx.DeviceMan()),
		window(gfx.Window()),
		swapchain_manager(gfx.SwapchainMan()),
		queue_manager(gfx.QueueMan()),
		mem_allocator(gfx.Instance(), gfx.DeviceMan()),
		sem_pool(device_manager),
		img_fac(gfx, mem_allocator, sem_pool),
		buf_fac(queue_manager, device_manager, mem_allocator, sem_pool),
		tex_fac(gfx, img_fac, buf_fac),
		shader_fac(gfx),

		descriptor_layout_manager(device_manager),

		gfx_pipe_CI_builder(gfx),
		pipeline_builder(device_manager, shader_fac),

		mat_metallic_roughness_producer(device_manager, descriptor_layout_manager, pipeline_builder),
		gltf_model_fac(device_manager, buf_fac, tex_fac, mat_metallic_roughness_producer)
	{
	}




	void IRenderer::RenderingPreparation()
	{
		//***************USER INPUT********************
		this->SetUpUserInput();
		//Init Camera
		this->CreateCamera();


		//**************SYNC OBJECTS INIT**************
		//prepare sync objects
		this->InitSynObjects();
		//**************GLOBAL RESOURCES***************
		//images
		this->CreateGlobalAttachments();
		this->CreateGlobalTextures();
		//buffers
		this->CreateGlobalBuffers();

		//DESCRIPTOR POOL
		//this->CreateCommonDescriptorPool();

		//*************MODEL INIT***************
		this->CreateGlobalModels();

		//*************RENDERPASS INIT***************
		this->InitRenderpasses();

		//*************COMMAND BUFFER RECORDING***************
		//command buffer recording
		//this->CommandBufferRecording();
	}

}        // namespace Anni
