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
	const std::string name_,
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

)
	: GraphicsPassNode(
		name_,
		device_manager_,
		swapchain_manager_,
		descriptor_set_layout_manager_,
		shader_fac_,
		descriptor_allocator_,
		vk_pipeline_builder_,
		rg_buffers_,
		rg_textures_,
		rg_name_2_vbuf_handle_,
		rg_name_2_vtex_handle_
	)

{
}

Anni::RenderGraphV1::PassType Anni::RenderGraphV1::PresentPass::GetRenderpassType()
{
	return  PassType::Present;
}






