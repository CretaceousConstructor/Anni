#pragma once


#include "IPassNode.h"

namespace Anni::RenderGraphV1
{
	class PresentPass final : public GraphicsPassNode
	{
	public:
		PresentPass(
			std::string name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			ShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			PipelineBuilder& vk_pipeline_builder_,
			std::vector<VirtualBuffer>& rg_buffers_,
			std::vector<VirtualTexture>& rg_textures_,

			std::unordered_map<std::string, VirtualBuffer::Handle>& rg_name_2_vbuf_handle_,
			std::unordered_map<std::string, VirtualTexture::Handle>& rg_name_2_vtex_handle_

		);


		PassType GetRenderpassType() override;

		void CreateDescriptorSetLayout() override;
		void AllocateDescriptorSets() override;
		void UpdateDescriptorSets() override;
		void GetShaders() override;
		void BeginRenderPass(vk::CommandBuffer cmd_buf) override;
		void EndRenderPass(vk::CommandBuffer cmd_buf) override;
		void RecordCommandBuffer(vk::CommandBuffer command_buffer) override;
	};


}
