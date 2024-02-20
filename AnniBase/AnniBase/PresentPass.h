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
			std::unordered_map<std::string, VirtualBuffer>& rg_buffers_map_,
			std::unordered_map<std::string, VirtualTexture>& rg_textures_map_);

		void UpdateDescriptorSets() override;
		void CreateDescriptorSetLayout() override;
		void GetShaders() override;
		void BeginRenderPass(vk::CommandBuffer cmd_buf) override;
		void EndRenderPass(vk::CommandBuffer cmd_buf) override;
		void RecordCommandBuffer(vk::CommandBuffer command_buffer) override;
		void AllocateDescriptorSets() override;
	};


}
