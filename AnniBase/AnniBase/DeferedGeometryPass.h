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
			const std::string& name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			ShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			PipelineBuilder& pipeline_builder_,
			std::vector<VirtualBuffer>& rg_buffers_,
			std::vector<VirtualTexture>& rg_textures_,
			std::unordered_map<std::string, VirtualBuffer::Handle>& rg_name_2_vbuf_handle_,
			std::unordered_map<std::string, VirtualTexture::Handle>& rg_name_2_vtex_handle_,
			GFXPipelineCIBuilder& gfx_pipelineCI_builder_);

	private:
		GFXPipelineCIBuilder& gfx_pipelineCI_builder;

	private:
		std::shared_ptr<ShaderWrapper> defefered_geometry_vert_shader;
		std::shared_ptr<ShaderWrapper> defefered_geometry_frag_shader;

	private:
		std::unique_ptr<GFXPipelineCI> gfx_pip_CI;
	private:
		std::shared_ptr<LoadedGLTF> sponza;


		PassType GetRenderpassType() override;
		void GetShaders() override;
		void RecordCommandBuffer(vk::CommandBuffer command_buffer) override;

	private:
		vk::UniquePipeline       defered_geometry_pipline;
		vk::UniquePipelineLayout defered_geometry_pipline_layout;

	};

}
