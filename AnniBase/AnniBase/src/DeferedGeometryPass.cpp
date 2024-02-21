#include "DeferedGeometryPass.h"

Anni::RenderGraphV1::DeferedGeometryPass::DeferedGeometryPass(
	std::string name_,
	DeviceManager& device_manager_,
	SwapchainManager& swapchain_manager_,
	DescriptorLayoutManager& descriptor_set_layout_manager_,
	VkShaderFactory& shader_fac_,
	DescriptorSetAllocatorGrowable& descriptor_allocator_,
	std::list<VirtualBuffer>& rg_buffers_,
	std::list<VirtualTexture>& rg_textures_,
	GFXPipelineCIBuilder& gfx_pipelineCI_builder_)
	: GraphicsPassNode(
		name_,
		device_manager_,
		swapchain_manager_,
		descriptor_set_layout_manager_,
		shader_fac_,
		descriptor_allocator_,
		rg_buffers_,
		rg_textures_),

	gfx_pipelineCI_builder(gfx_pipelineCI_builder_)
{
}



void Anni::RenderGraphV1::DeferedGeometryPass::UpdateDescriptorSets()
{
	std::vector<vk::WriteDescriptorSet> all_writes;
	for (auto& buf_usage : buf_usages)
	{
		if (Anni::RsrcType::Buffer == buf_usage.usage.GetRsrcType())
		{
			all_writes.push_back(
				buf_usage.v_rsrc->p_rsrc->GetWriteDescriptorSetInfo(
					buf_usage.usage, common_descriptor_set));
		}
	}

	for (auto& tex_usage : tex_usages)
	{
		if (std::holds_alternative<TexUsage>(tex_usage.usage))
		{
			TexUsage& texture_usage = std::get<TexUsage>(
				tex_usage.usage); // Only works if v holds a TexUsage
			if (Anni::RsrcType::Texture == texture_usage.GetRsrcType())
			{
				all_writes.push_back(
					tex_usage.v_rsrc->p_rsrc->GetWriteDescriptorSetInfo(
						texture_usage, common_descriptor_set));
			}
		}
	}

	device_manager.GetLogicalDevice().updateDescriptorSets(all_writes, {});
}

void Anni::RenderGraphV1::DeferedGeometryPass::CreateDescriptorSetLayout()
{
	std::vector<vk::DescriptorSetLayoutBinding> bindings;

	for (const auto& buf_usage : buf_usages)
	{
		if (Anni::RsrcType::Buffer == buf_usage.usage.GetRsrcType())
		{
			const Anni::DescriptorInfo desc_info = buf_usage.usage.desc_info;
			vk::DescriptorSetLayoutBinding one_binding(
				desc_info.slot_info.binding, desc_info.descriptor_type,
				Vk::DescriptorCount<1>, desc_info.desc_shader_stages_flags);
			bindings.push_back(one_binding);
		}
	}

	for (auto& tex_usage : tex_usages)
	{
		if (std::holds_alternative<TexUsage>(tex_usage.usage))
		{
			TexUsage& texture_usage = std::get<TexUsage>(
				tex_usage.usage); // Only works if v holds a TexUsage
			if (Anni::RsrcType::Texture == texture_usage.GetRsrcType())
			{
				const Anni::DescriptorInfo desc_info = texture_usage.desc_info;
				vk::DescriptorSetLayoutBinding one_binding(
					desc_info.slot_info.binding, desc_info.descriptor_type,
					Vk::DescriptorCount<1>, desc_info.desc_shader_stages_flags);

				bindings.push_back(one_binding);
			}
		}
	}

	vk::DescriptorSetLayoutCreateInfo desc_set_layout_CI{};
	desc_set_layout_CI.setBindings(bindings);

	common_layout =
		device_manager.GetLogicalDevice().createDescriptorSetLayoutUnique(
			desc_set_layout_CI);
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

void Anni::RenderGraphV1::DeferedGeometryPass::EndRenderPass(
	vk::CommandBuffer cmd_buf)
{
	cmd_buf.endRendering();
}

void Anni::RenderGraphV1::DeferedGeometryPass::BeginRenderPass(
	vk::CommandBuffer cmd_buf)
{
	vk::RenderingInfo rendering_info{};
	rendering_info.renderArea.offset = vk::Offset2D{ 0, 0 };
	rendering_info.renderArea.extent =
		swapchain_manager.GetSwapChainImageExtent2D();
	rendering_info.layerCount = 1;

	std::vector<vk::RenderingAttachmentInfo> color_attachment_infos;
	vk::RenderingAttachmentInfo depth_attachment_info;
	vk::RenderingAttachmentInfo stensil_attachment_info;

	// 先把attachment的sampler 和 view 创建好。
	for (auto& tex_usage : tex_usages)
	{
		if (std::holds_alternative<AttachUsage>(tex_usage.usage))
		{
			ImgViewAndSamplerGeneration(tex_usage);
		}
	}

	for (auto& tex_usage : tex_usages)
	{
		if (std::holds_alternative<AttachUsage>(tex_usage.usage))
		{
			tex_usage.BindResolveTarget();
			AttachUsage& attach_usage = std::get<AttachUsage>(
				tex_usage.usage); // Only works if v holds an AttachUsage
			const auto attach_type = attach_usage.attach_info.attach_type;
			if (Anni::AttachmentType::ColorAttachment == attach_type)
			{
				color_attachment_infos.push_back(
					attach_usage.GetVkRenderingAttachmentInfo());
			}
			if (Anni::AttachmentType::DepthAttachment == attach_type ||
				Anni::AttachmentType::DepthStencilAttachment == attach_type)
			{
				depth_attachment_info = attach_usage.GetVkRenderingAttachmentInfo();
			}
		}
	}
	rendering_info.setColorAttachments(color_attachment_infos);
	rendering_info.setPDepthAttachment(&depth_attachment_info);

	cmd_buf.beginRendering(rendering_info);
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
	for (auto& tex_usage : tex_usages)
	{
		if (std::holds_alternative<AttachUsage>(tex_usage.usage))
		{
			AttachUsage& attach_usage = std::get<AttachUsage>(
				tex_usage.usage); // Only works if v holds an AttachUsage
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
	DrawContext draw_context;
	const auto& model_textures_usage = model_to_multi_tex_usage.at(sponza.get());
	sponza->UpdateDescriptorSet(device_manager, model_textures_usage);
	std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> pipeline_pipeline_layout = sponza->BuildPipeline(*gfx_pip_CI);

	vk::PipelineLayout surface_pipeline_layout = pipeline_pipeline_layout.second.get();
	vk::Pipeline       surface_pipeline = pipeline_pipeline_layout.first.get();

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

	pipline_pool.push_back(std::move(pipeline_pipeline_layout.first));
	pipeline_layout_pool.push_back(std::move(pipeline_pipeline_layout.second));

}

void Anni::RenderGraphV1::DeferedGeometryPass::AllocateDescriptorSets()
{
	common_descriptor_set = descriptor_allocator.Allocate(common_layout.get());
}

void Anni::RenderGraphV1::DeferedGeometryPass::ImgViewAndSamplerGeneration(
	VirtualTexRsrcAndUsage& rsrc_usage)
{
	std::shared_ptr<VkTexture>& ptr_tex = rsrc_usage.v_rsrc->p_rsrc;
	std::variant<TexUsage, AttachUsage>& tex_usage = rsrc_usage.usage;

	std::visit(
		[&](auto& variant_usage)
		{
			if (!variant_usage.img_view_CI)
			{
				variant_usage.img_view = ptr_tex->GetTextureImageViewPtr();
			}
			else
			{
				variant_usage.img_view = std::make_shared<ImgViewWrapper>(
					device_manager, variant_usage.img_view_CI.value());
			}

			if (!variant_usage.sampler_CI)
			{
				variant_usage.sampler = ptr_tex->GetTextureSamplerPtr();
			}
			else
			{
				variant_usage.sampler = std::make_shared<SamplerWrapper>(
					device_manager, variant_usage.sampler_CI.value());
			}
		},
		tex_usage);
}
