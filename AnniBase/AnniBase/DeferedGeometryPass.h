#pragma once

#include "AnniMath.h"
#include "GFXPipelineCIBuilder.h"
#include "GLTFModelFactory.h"
#include "IPassNode.h"

namespace Anni::RenderGraphV1
{


	class DeferedGeometryPass final : public GraphicsPassNode
	{
	public:
		DeferedGeometryPass(
			std::string name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			VkShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			std::list<VirtualBuffer>& rg_buffers_,
			std::list<VirtualTexture>& rg_textures_,
			GFXPipelineCIBuilder& gfx_pipelineCI_builder_);

	public:
		vk::UniqueDescriptorSetLayout common_layout;
		vk::DescriptorSet common_descriptor_set;

	public:
		std::shared_ptr<VkShaderWrapper> defefered_geometry_vert_shader;
		std::shared_ptr<VkShaderWrapper> defefered_geometry_frag_shader;

	public:
		std::unique_ptr<GFXPipelineCI> gfx_pip_CI;
		std::shared_ptr<LoadedGLTF> sponza;

	public:
		GFXPipelineCIBuilder& gfx_pipelineCI_builder;

		void UpdateDescriptorSets() override;

		void CreateDescriptorSetLayout() override;

		void GetShaders() override;

		void EndRenderPass(vk::CommandBuffer cmd_buf) override;

		void BeginRenderPass(vk::CommandBuffer cmd_buf) override;

		void RecordCommandBuffer(vk::CommandBuffer command_buffer) override;

		void AllocateDescriptorSets() override;
	private:
		void ImgViewAndSamplerGeneration(VirtualTexRsrcAndUsage& rsrc_usage);
	private:
		std::vector<vk::UniquePipeline> pipline_pool;
		std::vector< vk::UniquePipelineLayout> pipeline_layout_pool;
	};

}
