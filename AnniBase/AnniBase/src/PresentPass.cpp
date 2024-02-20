#include "PresentPass.h"



void Anni::RenderGraphV1::PresentPass::BeginRenderPass(
	vk::CommandBuffer cmd_buf)
{
}

void Anni::RenderGraphV1::PresentPass::CreateDescriptorSetLayout()
{
}

void Anni::RenderGraphV1::PresentPass::UpdateDescriptorSets()
{
}
void Anni::RenderGraphV1::PresentPass::GetShaders()
{
}

void Anni::RenderGraphV1::PresentPass::EndRenderPass(
	vk::CommandBuffer cmd_buf)
{
}

void Anni::RenderGraphV1::PresentPass::RecordCommandBuffer(
	vk::CommandBuffer command_buffer)
{
}

void Anni::RenderGraphV1::PresentPass::AllocateDescriptorSets()
{
}

Anni::RenderGraphV1::PresentPass::PresentPass(
	std::string name_,
	DeviceManager& device_manager_,
	SwapchainManager& swapchain_manager_,
	DescriptorLayoutManager& descriptor_set_layout_manager_,
	VkShaderFactory& shader_fac_,
	DescriptorSetAllocatorGrowable& descriptor_allocator_,
	std::unordered_map<std::string, VirtualBuffer>& rg_buffers_map_,
	std::unordered_map<std::string, VirtualTexture>& rg_textures_map_)
	: GraphicsPassNode(
		name_,
		device_manager_,
		swapchain_manager_,
		descriptor_set_layout_manager_,
		shader_fac_,
		descriptor_allocator_,
		rg_buffers_map_,
		rg_textures_map_)

{
}






