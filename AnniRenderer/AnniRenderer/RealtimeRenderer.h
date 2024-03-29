#pragma once
#include "AnniBase/BaseRenderer.h"
#include "AnniBase/FirstPersonCamera.h"
#include "AnniBase/KeyBoardInputManager.h"
#include "AnniBase/RenderGraphV1.h"
// #include "IrradianceMapGenPass.h"
// #include "PrefilterAndLUTMapGenPass.h"
// #include "PbrRenderingPass.h"
#include "DeferedCompositionPass.h"
#include "DeferedGeometryPass.h"
// #include "MSAAPass.h"
#include "AnniBase/DeferedGeometryPass.h"
#include "AnniBase/DeferedCompositionPass.h"
#include "AnniBase/MouseInputManager.h"
#include "AnniBase/Vk.h"
#include "AnniBase/PresentPass.h"
#include "RenderingMetaInfo.h" 

#include <chrono>
#include <memory>
#include <optional>
#include <vector>






namespace Anni::Renderer
{

	class RealtimeRenderer final : public IRenderer
	{

	public:
		struct FrameData
		{
			std::shared_ptr<Texture> swapchain_attachment;
			std::shared_ptr<Buffer>    uniform_buffer_gpu_MSAA;
			MSAA::UBO                  matrix_buffer_cpu_MSAA;
		};

	public:
		RealtimeRenderer(
			Anni::GraphicsComponent& gfx_
		) :
			IRenderer(gfx_),
			descriptor_allocator(device_manager),
			render_graph_v1(
				device_manager,
				swapchain_manager,
				queue_manager,
				descriptor_layout_manager,
				descriptor_allocator,
				sem_pool,
				buf_fac,
				tex_fac,
				shader_fac,
				gfx_pipe_CI_builder,
				pipeline_builder
			)
		{
		}

		~RealtimeRenderer() override = default;


	public:
		void Render(float time_diff) override;
	private:
		void RenderGraphRendering(uint32_t img_index, uint32_t cur_frame);


	public:
		void SetUpUserInput() override;
		void CreateCamera() override;

		void InitSynObjects() override;

		void CreateGlobalAttachments() override;
		void CreateGlobalTextures() override;
		void CreateGlobalBuffers() override;

		//void CreateCommonDescriptorPool() override;

		void CreateGlobalModels() override;

		void InitRenderpasses() override;

		//void CommandBufferRecording() override;


	private:
		void UpdateUniformBuffer(size_t current_image_index) override;
		void UpdateCamera(float dt) override;


	private:
		//void CreateDepthTextures();
		void CreateSwapchainTextures();

		static void ImportModelRsrcToRenderGraph(
			Anni::LoadedGLTF& model,
			Anni::RenderGraphV1::GraphicsPassNode& pass_node
		);

	private:
		// INPUT MANAGER
		std::unique_ptr<::Anni::KeyBoardInputManager> keyboard;
		std::unique_ptr<::Anni::MouseInputManager> mouse;
		// CAMERA
		std::unique_ptr<::Anni::FirstPersonCamera> camera;


		// SYN OBJECTS
		std::array< std::shared_ptr<TimelineSemWrapper>, Vk::MAX_FRAMES_OVERLAP> frame_num_semaphores;
		std::vector<std::shared_ptr<BinarySemWrapper>>  present_finished_semaphores;
		std::vector<std::shared_ptr<BinarySemWrapper>>  ready_present_semaphores;
		//*********************************************************************************************
		std::vector<std::shared_ptr<TimelineSemWrapper>>  swap_img_rdy_4_rendering_helper_sem;
		std::vector<std::shared_ptr<TimelineSemWrapper>>  present_finished_helper_sem;

		// UI
		//Anni::VkImgui imgui_UI;
		//std::vector<std::shared_ptr<BinarySemWrapper>>  UI_rendering_finished_semaphores;

		// FRAME DATA
		std::array<FrameData, Vk::SWAP_IMG_COUNT> frame_datas;

		// MODEL
		std::shared_ptr<Anni::LoadedGLTF> test_model_sponza;

		//DESCRIPTOR SET ALLOCATOR
		Anni::DescriptorSetAllocatorGrowable descriptor_allocator;

		// RENDER GRAPH
		// you need to re-establish render graph for EVERY FRAME
		Anni::RenderGraphV1::DependencyGraph render_graph_v1;

	private:

		RealtimeRenderer() = delete;
		RealtimeRenderer(const RealtimeRenderer&) = delete;
		RealtimeRenderer& operator=(const RealtimeRenderer&) = delete;
		RealtimeRenderer(RealtimeRenderer&&) = delete;
		RealtimeRenderer& operator=(RealtimeRenderer&&) = delete;
	};

}

