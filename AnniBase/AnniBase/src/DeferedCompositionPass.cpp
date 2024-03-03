#include "DeferedCompositionPass.h"

Anni::RenderGraphV1::DeferedCompositionPass::DeferedCompositionPass(
	const std::string& name_,
	DeviceManager& device_manager_,
	SwapchainManager& swapchain_manager_,
	DescriptorLayoutManager& descriptor_set_layout_manager_,
	ShaderFactory& shader_fac_,
	DescriptorSetAllocatorGrowable& descriptor_allocator_,
	PipelineBuilder&             pipeline_builder_,
	std::vector<VirtualBuffer>& rg_buffers_,
	std::vector<VirtualTexture>& rg_textures_,
	std::unordered_map<std::string, VirtualBuffer::Handle>& rg_name_2_vbuf_handle_,       
	std::unordered_map<std::string, VirtualTexture::Handle>& rg_name_2_vtex_handle_,
	GFXPipelineCIBuilder& gfx_pipelineCI_builder_
)
	: GraphicsPassNode(
		name_,
		device_manager_,
		swapchain_manager_,
		descriptor_set_layout_manager_,
		shader_fac_,
		descriptor_allocator_,
		pipeline_builder_,
		rg_buffers_,
		rg_textures_,
		rg_name_2_vbuf_handle_,
		rg_name_2_vtex_handle_
	),
	gfx_pipelineCI_builder(gfx_pipelineCI_builder_)
{
}

Anni::RenderGraphV1::PassType Anni::RenderGraphV1::DeferedCompositionPass::GetRenderpassType()
{
	return PassType::Graphics;
}

void Anni::RenderGraphV1::DeferedCompositionPass::GetShaders()
{
	defefered_composition_stencilwrite_vert_shader = shader_fac.GetShader(
		::std::string("../../data/shaders/DeferedRendering/DeferedComStencilWrite_vert.hlsl"),
		VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_stencilwrite_frag_shader = shader_fac.GetShader(
		::std::string("../../data/shaders/DeferedRendering/DeferedComStencilWrite_frag.hlsl"),
		VK_SHADER_STAGE_FRAGMENT_BIT);

	defefered_composition_per_sample_vert_shader = shader_fac.GetShader(
		::std::string("../../data/shaders/DeferedRendering/DeferedComPerSample_vert.hlsl"),
		VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_per_sample_frag_shader = shader_fac.GetShader(
		::std::string("../../data/shaders/DeferedRendering/DeferedComPerSample_frag.hlsl"),
		VK_SHADER_STAGE_FRAGMENT_BIT);

	defefered_composition_per_frag_vert_shader = shader_fac.GetShader(
		::std::string("../../data/shaders/DeferedRendering/DeferedComPerFrag_vert.hlsl"),
		VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_per_frag_frag_shader = shader_fac.GetShader(
		::std::string("../../data/shaders/DeferedRendering/DeferedComPerFrag_frag.hlsl"),
		VK_SHADER_STAGE_FRAGMENT_BIT);
}








void Anni::RenderGraphV1::DeferedCompositionPass::RecordCommandBuffer(
	vk::CommandBuffer command_buffer)
{
	//STENCIL WRITE（通过写入模板值区分复杂或者处在边缘的pixel和不在边缘的pixel）
	{
		gfx_pipelineCI_builder.RestoreToDefaultState(*gfx_pip_CI);
		//SHADERS INSTALLATION
		const std::vector shader_stages{ defefered_composition_stencilwrite_vert_shader->GetShaderStageCI(),defefered_composition_stencilwrite_frag_shader->GetShaderStageCI() };

		// DYNAMIC RENDERING FORMATS
		std::vector<DynamicRenderingAttachmentFormatInfo> dynamic_rendering_attach_formats;
		for (auto& out_tex : outs_tex)
		{
			if (std::holds_alternative<AttachUsage>(out_tex.usage))
			{
				AttachUsage& attach_usage = std::get<AttachUsage>(out_tex.usage); // Only works if v holds an AttachUsage
				dynamic_rendering_attach_formats.push_back(
					attach_usage.GetDynamicRenderingAttachmentFormatInfo());
			}
		}

		gfx_pip_CI->multisample_state_CI.rasterizationSamples = vk::SampleCountFlagBits::e4;
		gfx_pip_CI->multisample_state_CI.sampleShadingEnable = VK_FALSE;

		//To correctly draw screen quad, we need:
		gfx_pip_CI->rasterization_state_CI.cullMode = vk::CullModeFlagBits::eFront;
		gfx_pip_CI->rasterization_state_CI.frontFace = vk::FrontFace::eCounterClockwise;

		//To use stencil buffer, we neeed:
		gfx_pip_CI->depth_stencil_CI.stencilTestEnable = vk::True;
		//VK_COMPARE_OP_ALWAYS specifies that the comparison always evaluates true.
		gfx_pip_CI->depth_stencil_CI.back.compareOp = vk::CompareOp::eAlways;
		gfx_pip_CI->depth_stencil_CI.back.compareMask = 0xff;
		gfx_pip_CI->depth_stencil_CI.back.failOp = vk::StencilOp::eReplace;
		gfx_pip_CI->depth_stencil_CI.back.depthFailOp = vk::StencilOp::eReplace;
		gfx_pip_CI->depth_stencil_CI.back.passOp = vk::StencilOp::eReplace;
		gfx_pip_CI->depth_stencil_CI.back.writeMask = 0xff;
		gfx_pip_CI->depth_stencil_CI.back.reference = 1;

		gfx_pip_CI->depth_stencil_CI.front = gfx_pip_CI->depth_stencil_CI.back;
		//sa = (sa & ~sw) | (sg & sw)
		//sa = (0x00 & ~0xFF)  | ( sg & 0xFF)

		vk::Viewport viewport{};
		vk::Rect2D   scissor{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.width = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width);
		viewport.height = -static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = vk::Offset2D{ 0, 0 };
		scissor.extent = VkExtent2D{ swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height };

		gfx_pip_CI->view_port_scissor_pair.first[0] = viewport;
		gfx_pip_CI->view_port_scissor_pair.second[0] = scissor;

		gfx_pip_CI->SetPipelineShaderStageCreateInfo(shader_stages);
		gfx_pip_CI->SetDynamicRenderingAttachmentFormats(dynamic_rendering_attach_formats);
		gfx_pip_CI->AddDescriptorSetLayout(common_layout.get());


		std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> pipeline_pipeline_layout = pipeline_builder.BuildPipeline(*gfx_pip_CI);
		const vk::PipelineLayout surface_pipeline_layout = pipeline_pipeline_layout.second.get();
		const vk::Pipeline       surface_pipeline = pipeline_pipeline_layout.first.get();
		{
			//*********************Stencil Writing************************************
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, surface_pipeline);
			//BIND COMMON DESCRIPTOR SET
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, surface_pipeline_layout, 0, common_descriptor_set, {});

			//DRAWING QUAD
			command_buffer.draw(3, 1, 0, 0);
		}
		composition_stencilwrite_pipeline = std::move(pipeline_pipeline_layout.first);
		composition_stencilwrite_pipe_layout = std::move(pipeline_pipeline_layout.second);
	}





	//PER FRAGMENT SHADING
	{
		gfx_pipelineCI_builder.RestoreToDefaultState(*gfx_pip_CI);
		//SHADERS INSTALLATION
		const std::vector shader_stages{ defefered_composition_per_frag_vert_shader->GetShaderStageCI(), defefered_composition_per_frag_frag_shader->GetShaderStageCI() };

		// DYNAMIC RENDERING FORMATS
		std::vector<DynamicRenderingAttachmentFormatInfo> dynamic_rendering_attach_formats;
		for (auto& out_tex : outs_tex)
		{
			if (std::holds_alternative<AttachUsage>(out_tex.usage))
			{
				AttachUsage& attach_usage = std::get<AttachUsage>(out_tex.usage); // Only works if v holds an AttachUsage
				dynamic_rendering_attach_formats.push_back(attach_usage.GetDynamicRenderingAttachmentFormatInfo());
			}
		}

		gfx_pip_CI->multisample_state_CI.sampleShadingEnable = VK_FALSE;
		gfx_pip_CI->multisample_state_CI.rasterizationSamples = vk::SampleCountFlagBits::e4;
		//To correctly draw screen quad.

		gfx_pip_CI->rasterization_state_CI.cullMode = vk::CullModeFlagBits::eFront;
		gfx_pip_CI->rasterization_state_CI.frontFace = vk::FrontFace::eCounterClockwise;
		//To Use stencil buffer
		gfx_pip_CI->depth_stencil_CI.stencilTestEnable = vk::True;

		gfx_pip_CI->depth_stencil_CI.back.compareOp = vk::CompareOp::eEqual;
		gfx_pip_CI->depth_stencil_CI.back.compareMask = 0xff;
		gfx_pip_CI->depth_stencil_CI.back.failOp = vk::StencilOp::eKeep;
		gfx_pip_CI->depth_stencil_CI.back.depthFailOp = vk::StencilOp::eKeep;
		gfx_pip_CI->depth_stencil_CI.back.passOp = vk::StencilOp::eKeep;
		gfx_pip_CI->depth_stencil_CI.back.writeMask = 0xff;
		gfx_pip_CI->depth_stencil_CI.back.reference = 0;

		gfx_pip_CI->depth_stencil_CI.front = gfx_pip_CI->depth_stencil_CI.back;
		//sa = (sa & ~sw) | (sg & sw)
		//sa = (0x00 & ~0xFF)  | ( sg & 0xFF)

		vk::Viewport viewport{};
		vk::Rect2D   scissor{};

		viewport.x = 0.0f;
		viewport.y = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.width = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width);
		viewport.height = -static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = vk::Offset2D{ 0, 0 };
		scissor.extent = VkExtent2D{ swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height };

		gfx_pip_CI->view_port_scissor_pair.first[0] = viewport;
		gfx_pip_CI->view_port_scissor_pair.second[0] = scissor;

		gfx_pip_CI->SetPipelineShaderStageCreateInfo(shader_stages);
		gfx_pip_CI->SetDynamicRenderingAttachmentFormats(dynamic_rendering_attach_formats);
		gfx_pip_CI->AddDescriptorSetLayout(common_layout.get());



		std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> pipeline_pipeline_layout = pipeline_builder.BuildPipeline(*gfx_pip_CI);
		const vk::PipelineLayout surface_pipeline_layout = pipeline_pipeline_layout.second.get();
		const vk::Pipeline       surface_pipeline = pipeline_pipeline_layout.first.get();
		{
			//*********************Stencil Writing************************************
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, surface_pipeline);
			//BIND COMMON DESCRIPTOR SET
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, surface_pipeline_layout, 0, common_descriptor_set, {});
			//DRAWING QUAD
			command_buffer.draw(3, 1, 0, 0);
		}

		composition_per_frag_pipeline = std::move(pipeline_pipeline_layout.first);
		composition_per_frag_pipe_layout = std::move(pipeline_pipeline_layout.second);
	}


	//PER SAMPLE SHADING（这里最后一个pipeline绑定然后运行完应当还需要做resolve，具体做法参见笔记MSAA）
	{
		gfx_pipelineCI_builder.RestoreToDefaultState(*gfx_pip_CI);
		//SHADERS INSTALLATION
		const ::std::vector shader_stages{ defefered_composition_per_sample_vert_shader->GetShaderStageCI(), defefered_composition_per_sample_frag_shader->GetShaderStageCI() };

		// DYNAMIC RENDERING FORMATS
		std::vector<DynamicRenderingAttachmentFormatInfo> dynamic_rendering_attach_formats;
		for (auto& out_tex : outs_tex)
		{
			if (std::holds_alternative<AttachUsage>(out_tex.usage))
			{
				AttachUsage& attach_usage = std::get<AttachUsage>(out_tex.usage); // Only works if v holds an AttachUsage
				dynamic_rendering_attach_formats.push_back(attach_usage.GetDynamicRenderingAttachmentFormatInfo());
			}
		}

		//To turn on per sample shading
		gfx_pip_CI->multisample_state_CI.rasterizationSamples = vk::SampleCountFlagBits::e4;
		gfx_pip_CI->multisample_state_CI.sampleShadingEnable = vk::True;
		gfx_pip_CI->multisample_state_CI.minSampleShading = 1.0f;

		//To correctly draw screen quad.

		gfx_pip_CI->rasterization_state_CI.cullMode = vk::CullModeFlagBits::eFront;
		gfx_pip_CI->rasterization_state_CI.frontFace = vk::FrontFace::eCounterClockwise;
		//Use stencil buffer
		gfx_pip_CI->depth_stencil_CI.stencilTestEnable = vk::True;


		gfx_pip_CI->depth_stencil_CI.back.compareOp = vk::CompareOp::eEqual;
		gfx_pip_CI->depth_stencil_CI.back.compareMask = 0xff;

		gfx_pip_CI->depth_stencil_CI.back.failOp = vk::StencilOp::eKeep;
		gfx_pip_CI->depth_stencil_CI.back.depthFailOp = vk::StencilOp::eKeep;
		gfx_pip_CI->depth_stencil_CI.back.passOp = vk::StencilOp::eKeep;
		gfx_pip_CI->depth_stencil_CI.back.writeMask = 0xff;
		gfx_pip_CI->depth_stencil_CI.back.reference = 1;

		gfx_pip_CI->depth_stencil_CI.front = gfx_pip_CI->depth_stencil_CI.back;
		//sa = (sa & ~sw) | (sg & sw)
		//sa = (0x00 & ~0xFF)  | ( sg & 0xFF)

		VkViewport viewport{};
		VkRect2D   scissor{};

		viewport.x = 0.0f;
		viewport.y = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.width = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width);
		viewport.height = -static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = { 0, 0 };
		scissor.extent = VkExtent2D{ swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height };

		gfx_pip_CI->view_port_scissor_pair.first[0] = viewport;
		gfx_pip_CI->view_port_scissor_pair.second[0] = scissor;

		gfx_pip_CI->SetPipelineShaderStageCreateInfo(shader_stages);
		gfx_pip_CI->SetDynamicRenderingAttachmentFormats(dynamic_rendering_attach_formats);
		gfx_pip_CI->AddDescriptorSetLayout(common_layout.get());

		std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> pipeline_pipeline_layout = pipeline_builder.BuildPipeline(*gfx_pip_CI);
		const vk::PipelineLayout surface_pipeline_layout = pipeline_pipeline_layout.second.get();
		const vk::Pipeline       surface_pipeline = pipeline_pipeline_layout.first.get();
		{
			//*********************Stencil Writing************************************
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, surface_pipeline);
			//BIND COMMON DESCRIPTOR SET
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, surface_pipeline_layout, 0, common_descriptor_set, {});
			//DRAWING QUAD
			command_buffer.draw(3, 1, 0, 0);
		}
		composition_per_sample_pipeline = std::move(pipeline_pipeline_layout.first);
		composition_per_sample_pipe_layout = std::move(pipeline_pipeline_layout.second);
	}









}























