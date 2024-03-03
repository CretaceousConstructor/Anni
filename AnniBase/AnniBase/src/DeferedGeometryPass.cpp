#include "DeferedGeometryPass.h"

Anni::RenderGraphV1::DeferedGeometryPass::DeferedGeometryPass(
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

	GFXPipelineCIBuilder& gfx_pipelineCI_builder_)
	: GraphicsPassNode(
		std::move(name_),
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


Anni::RenderGraphV1::PassType Anni::RenderGraphV1::DeferedGeometryPass::GetRenderpassType()
{
	return PassType::Graphics;
}

void Anni::RenderGraphV1::DeferedGeometryPass::GetShaders()
{
	// these shaders can be cached in std map
	defefered_geometry_vert_shader = shader_fac.GetShader(
		std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/"
			"DeferedGeoPass_vert.hlsl"),
		VK_SHADER_STAGE_VERTEX_BIT);
	defefered_geometry_frag_shader = shader_fac.GetShader(
		std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/"
			"DeferedGeoPass_frag.hlsl"),
		VK_SHADER_STAGE_FRAGMENT_BIT);
}



void Anni::RenderGraphV1::DeferedGeometryPass::RecordCommandBuffer(
	vk::CommandBuffer command_buffer)
{
	// SHADERS
	const std::vector<vk::PipelineShaderStageCreateInfo> shader_stages{
		defefered_geometry_vert_shader->GetShaderStageCI(),
		defefered_geometry_frag_shader->GetShaderStageCI() };

	// DYNAMIC RENDERING FORMATS
	std::vector<DynamicRenderingAttachmentFormatInfo>
		dynamic_rendering_attach_formats;


	for (auto& out_tex : outs_tex)
	{
		if (std::holds_alternative<AttachUsage>(out_tex.usage))
		{
			AttachUsage& attach_usage = std::get<AttachUsage>(out_tex.usage); // Only works if v holds an AttachUsage
			dynamic_rendering_attach_formats.push_back(
				attach_usage.GetDynamicRenderingAttachmentFormatInfo());
		}
	}


	gfx_pip_CI = gfx_pipelineCI_builder.GetPipelineCI();
	// for reversed Z value

	gfx_pip_CI->depth_stencil_CI.depthCompareOp = vk::CompareOp::eGreaterOrEqual;

	// multisample
	gfx_pip_CI->multisample_state_CI.rasterizationSamples =
		vk::SampleCountFlagBits::e4;
	gfx_pip_CI->multisample_state_CI.sampleShadingEnable = vk::False;

	gfx_pip_CI->SetDynamicRenderingAttachmentFormats(
		dynamic_rendering_attach_formats);
	gfx_pip_CI->SetPipelineShaderStageCreateInfo(shader_stages);
	gfx_pip_CI->AddDescriptorSetLayout(common_layout.get());



	// Model Drawing(Sponza)
	DrawContext draw_context{};
	const auto& model_textures_usage = model_to_multi_tex_usage.at(sponza.get());
	sponza->UpdateDescriptorSet(device_manager, model_textures_usage);
	std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> pipeline_pipeline_layout = sponza->BuildPipeline(*gfx_pip_CI);

	const vk::PipelineLayout surface_pipeline_layout = pipeline_pipeline_layout.second.get();
	const vk::Pipeline       surface_pipeline = pipeline_pipeline_layout.first.get();

	{
		sponza->Draw(glm::mat4(1.0f), draw_context);
		for (const auto& r : draw_context.OpaqueSurfaces)
		{

			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, surface_pipeline);
			// Bind common descriptor set(not related to model)
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, surface_pipeline_layout, 0, common_descriptor_set, {});
			// Bind common descriptor set(related to model)
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, surface_pipeline_layout, 1, r.material->materialSet, {});

			command_buffer.bindIndexBuffer(r.indexBuffer->GetGPUBuffer(), Vk::Offset<0>, vk::IndexType::eUint32);

			// calculate final mesh matrix
			GPUDrawPushConstants push_constants;
			push_constants.worldMatrix = r.transform;
			push_constants.vertexBuffer = r.vertexBufferAddress;

			command_buffer.pushConstants<GPUDrawPushConstants>(surface_pipeline_layout, GPUDrawPushConstants::UseStages, Vk::Offset<0>, push_constants);
			command_buffer.drawIndexed(r.indexCount, 1, r.firstIndex, 0, 0);
		}
	}

	defered_geometry_pipline = std::move(pipeline_pipeline_layout.first);
	defered_geometry_pipline_layout = std::move(pipeline_pipeline_layout.second);
}


