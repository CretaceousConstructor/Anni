#pragma once
#include "AnniMath.h"
#include "GraphicsComponent.h"
#include "TextureFactory.h"
#include "BufferFactory.h" 
#include "GLTFModelFactory.h"
#include "GFXPipelineCIBuilder.h"

#include <array>
#include <chrono>

#include <iostream>
#include <memory>
#include <random>
#include <vector>


namespace Anni::Renderer
{
	class IRenderer
	{

	public:
		explicit IRenderer(GraphicsComponent& gfx_);
		virtual ~IRenderer() = default;
		void RenderingPreparation();

	public:
		virtual void Render(float time_diff) = 0;

	protected:
		//GFX COMPONENTS
		GraphicsComponent& gfx;
		DeviceManager& device_manager;
		WindowsSys& window;
		SwapchainManager& swapchain_manager;
		QueueManager& queue_manager;


		//MEM ALLOCATOR
		VmaAllocatorWrapper mem_allocator;

		//POOLS
		VkTimelineSemPoolUnsafe sem_pool;

		//FACTORIES
		ImageFactory img_fac;
		BufferFactory buf_fac;
		VkTextureFactory tex_fac;
		VkShaderFactory shader_fac;

		//MANAGERS
		DescriptorLayoutManager descriptor_layout_manager;

		//BUILDERS
		GFXPipelineCIBuilder gfx_pipe_CI_builder;
		VkPipelineBuilder pipeline_builder;

		//MATERIALS
		GLTFMetallicRoughnessProducer mat_metallic_roughness_producer;
		//MODELS
		GLTFModelFactory gltf_model_fac;

	protected:
		virtual void SetUpUserInput() = 0;
		virtual void CreateCamera() = 0;

		virtual void CreateGlobalAttachments() = 0;
		virtual void CreateGlobalTextures() = 0;
		virtual void CreateGlobalBuffers() = 0;

		//virtual void CreateCommonDescriptorPool() = 0;

		virtual void CreateGlobalModels() = 0;
		virtual void InitRenderpasses() = 0;
		virtual void InitSynObjects() = 0;

		//virtual void CommandBufferRecording() = 0;


	protected:
		virtual void UpdateUniformBuffer(size_t currentImage) = 0;
		virtual void UpdateCamera(float dt) = 0;



		IRenderer() = delete;

		IRenderer(const IRenderer&) = delete;
		IRenderer& operator=(const IRenderer&) = delete;

		IRenderer(IRenderer&&) = delete;
		IRenderer& operator=(IRenderer&&) = delete;
	};
}        // namespace Anni
