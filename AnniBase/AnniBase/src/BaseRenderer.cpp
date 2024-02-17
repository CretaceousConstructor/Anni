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
		sem_pool(device_manager),
		img_fac(gfx),
		buf_fac(device_manager, sem_pool),
		tex_fac(gfx, img_fac, buf_fac),
		shader_fac(gfx),

		descriptor_layout_manager(device_manager),

		gfx_pipe_CI_builder(gfx),
		pipeline_builder(device_manager, shader_fac),

		mat_metallic_roughness(device_manager, descriptor_layout_manager, pipeline_builder, gfx_pipe_CI_builder),
		gltf_model_fac(device_manager, buf_fac, tex_fac, mat_metallic_roughness)
	{
	}




	void IRenderer::RenderingPreparation()
	{
		//***************USER INPUT***************
		this->SetUpUserInput();
		//Init Camera
		this->CreateCamera();

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

		//prepare sync objects
		this->InitSynObjects();

		//*************COMMAND BUFFER RECORDING***************
		//command buffer recording
		//this->CommandBufferRecording();
	}

}        // namespace Anni
