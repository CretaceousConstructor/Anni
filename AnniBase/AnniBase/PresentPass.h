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
			VkShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			std::list<VirtualBuffer>& rg_buffers_,
			std::list<VirtualTexture>& rg_textures_);

		void UpdateDescriptorSets() override;
		void CreateDescriptorSetLayout() override;
		void GetShaders() override;
		void BeginRenderPass(vk::CommandBuffer cmd_buf) override;
		void EndRenderPass(vk::CommandBuffer cmd_buf) override;
		void RecordCommandBuffer(vk::CommandBuffer command_buffer) override;
		void AllocateDescriptorSets() override;
	};


}
