#pragma once
#include "AnniMath.h"
#include "GFXPipelineCIBuilder.h"
#include "GLTFModelFactory.h"
#include "IPassNode.h"

namespace Anni::RenderGraphV1
{
	class DeferedCompositionPass final : public GraphicsPassNode
	{
	public:
		DeferedCompositionPass(
			const std::string& name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			VkShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			VkPipelineBuilder& pipeline_builder_,
			std::vector<VirtualBuffer>& rg_buffers_,
			std::vector<VirtualTexture>& rg_textures_,
			GFXPipelineCIBuilder& gfx_pipelineCI_builder_);

	public:
		//Shaders
		std::shared_ptr<VkShaderWrapper> defefered_composition_stencilwrite_vert_shader;
		std::shared_ptr<VkShaderWrapper> defefered_composition_stencilwrite_frag_shader;
		//
		std::shared_ptr<VkShaderWrapper> defefered_composition_per_frag_vert_shader;
		std::shared_ptr<VkShaderWrapper> defefered_composition_per_frag_frag_shader;

		//
		std::shared_ptr<VkShaderWrapper> defefered_composition_per_sample_vert_shader;
		std::shared_ptr<VkShaderWrapper> defefered_composition_per_sample_frag_shader;


	public:
		std::unique_ptr<GFXPipelineCI> gfx_pip_CI;
	public:
		GFXPipelineCIBuilder& gfx_pipelineCI_builder;

		void GetShaders() override;
		void RecordCommandBuffer(vk::CommandBuffer command_buffer) override;

		vk::UniquePipeline composition_stencilwrite_pipeline;
		vk::UniquePipelineLayout composition_stencilwrite_pipe_layout;

		vk::UniquePipeline composition_per_frag_pipeline;
		vk::UniquePipelineLayout composition_per_frag_pipe_layout;

		vk::UniquePipeline composition_per_sample_pipeline;
		vk::UniquePipelineLayout composition_per_sample_pipe_layout;

	};
}
