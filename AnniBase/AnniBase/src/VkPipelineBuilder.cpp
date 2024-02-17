#include "VkPipelineBuilder.h"
namespace Anni
{

	VkPipelineBuilder::VkPipelineBuilder(DeviceManager& device_manager_, VkShaderFactory& shader_factory_) :
		device_manager(device_manager_)
	{
	}


	/// <summary>
	/// 填写所有CI，然后创建 
	/// </summary>
	/// <param name="local_pip_CI"></param>
	/// <returns></returns>
	std::pair<std::unique_ptr<PipelineWrapper>, std::unique_ptr<PipelineLayoutWrapper>> VkPipelineBuilder::BuildPipeline(GFXPipelineCI local_pip_CI) const
	{

		//******************error detecting******************

		//COLOR BLEND
		local_pip_CI.color_blend_state_CI.attachmentCount = static_cast<uint32_t>(local_pip_CI.color_blend_attachments.size());
		local_pip_CI.color_blend_state_CI.pAttachments = local_pip_CI.color_blend_attachments.data();
		//VERTEX INPUT
		local_pip_CI.vertex_input_state_CI.vertexBindingDescriptionCount = static_cast<uint32_t>(local_pip_CI.vertex_input_binding_descriptions.size());
		local_pip_CI.vertex_input_state_CI.pVertexBindingDescriptions = local_pip_CI.vertex_input_binding_descriptions.data();
		local_pip_CI.vertex_input_state_CI.vertexAttributeDescriptionCount = static_cast<uint32_t>(local_pip_CI.vertex_input_attribute_description.size());
		local_pip_CI.vertex_input_state_CI.pVertexAttributeDescriptions = local_pip_CI.vertex_input_attribute_description.data();

		//DYNAMIC RENDERING ATTACHMENT FORMATS 
		std::vector<VkFormat> color_attachments;

		for (const auto& format : local_pip_CI.attachment_formats)
		{
			if (AttachmentType::ColorAttachment == format.attach_type)
			{
				color_attachments.push_back(format.format);
			}

			if (AttachmentType::DepthAttachment == format.attach_type)
			{
				local_pip_CI.pipeline_rendering_CI.depthAttachmentFormat = format.format;
				local_pip_CI.pipeline_rendering_CI.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
			}

			if (AttachmentType::DepthStencilAttachment == format.attach_type)
			{
				local_pip_CI.pipeline_rendering_CI.depthAttachmentFormat = format.format;
				local_pip_CI.pipeline_rendering_CI.stencilAttachmentFormat = format.format;
			}
		}

		local_pip_CI.pipeline_rendering_CI.colorAttachmentCount = static_cast<uint32_t>(color_attachments.size());
		local_pip_CI.pipeline_rendering_CI.pColorAttachmentFormats = color_attachments.data();
		local_pip_CI.pipeline_rendering_CI.viewMask = 0;


		//COLOR BLEND AND COLOR ATTACH MUST HAVE THE SAME NUMBER
		if (local_pip_CI.pipeline_rendering_CI.colorAttachmentCount != local_pip_CI.color_blend_state_CI.attachmentCount && local_pip_CI.color_blend_state_CI.attachmentCount == 1)
		{
			const auto first_color_blend_attachment = local_pip_CI.color_blend_attachments.front();
			local_pip_CI.color_blend_attachments.resize(local_pip_CI.pipeline_rendering_CI.colorAttachmentCount, first_color_blend_attachment);

			local_pip_CI.color_blend_state_CI.attachmentCount = static_cast<uint32_t>(local_pip_CI.color_blend_attachments.size());
			local_pip_CI.color_blend_state_CI.pAttachments = local_pip_CI.color_blend_attachments.data();
			assert(local_pip_CI.pipeline_rendering_CI.colorAttachmentCount == local_pip_CI.color_blend_state_CI.attachmentCount);
		}
		else
		{
			//The spec says: VkPipelineRenderingCreateInfoKHR::colorAttachmentCount (3) must equal pColorBlendState->attachmentCount (1)
			assert(local_pip_CI.pipeline_rendering_CI.colorAttachmentCount == local_pip_CI.color_blend_state_CI.attachmentCount);
		}


		//INSTALL SPECILIZATION INFOS FOR EVERY SHADER
		for (auto& shaderStage : local_pip_CI.shader_stage_CI)
		{
			auto it = std::ranges::find_if(local_pip_CI.specialization_infos,
				[&shaderStage](const auto& specialization)
				{
					return specialization.shader_stage == shaderStage.stage;
				});

			if (it != local_pip_CI.specialization_infos.end())
			{
				shaderStage.pSpecializationInfo = &it->sp_info;
			}
		}

		//DYNAMIC STATES
		local_pip_CI.dynamic_state_CI.pDynamicStates = local_pip_CI.dynamic_states.data();
		local_pip_CI.dynamic_state_CI.dynamicStateCount = static_cast<uint32_t>(local_pip_CI.dynamic_states.size());

		//PIPLINE CI (use dynamic rendering)
		VkGraphicsPipelineCreateInfo pipeline_CI;
		pipeline_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_CI.flags = 0;

		pipeline_CI.renderPass = VK_NULL_HANDLE;        //use dynamic rendering
		pipeline_CI.subpass = 0;                        //use dynamic rendering

		pipeline_CI.basePipelineIndex = -1;
		pipeline_CI.basePipelineHandle = VK_NULL_HANDLE;


		//VIEWPORT AND SCISSOR
		local_pip_CI.viewport_state_CI.viewportCount = static_cast<uint32_t>(local_pip_CI.view_port_scissor_pair.first.size());
		local_pip_CI.viewport_state_CI.pViewports = local_pip_CI.view_port_scissor_pair.first.data();

		local_pip_CI.viewport_state_CI.scissorCount = static_cast<uint32_t>(local_pip_CI.view_port_scissor_pair.second.size());
		local_pip_CI.viewport_state_CI.pScissors = local_pip_CI.view_port_scissor_pair.second.data();


		//PIPELINE LAYOUT
		auto pipeline_layout = BuildPipelineLayout(local_pip_CI);

		//FINALIZE
		pipeline_CI.pInputAssemblyState = &local_pip_CI.input_assembly_state_CI;
		pipeline_CI.pRasterizationState = &local_pip_CI.rasterization_state_CI;
		pipeline_CI.pDepthStencilState = &local_pip_CI.depth_stencil_CI;
		pipeline_CI.pMultisampleState = &local_pip_CI.multisample_state_CI;
		pipeline_CI.pDynamicState = &local_pip_CI.dynamic_state_CI;        // Optional
		pipeline_CI.pColorBlendState = &local_pip_CI.color_blend_state_CI;
		pipeline_CI.pViewportState = &local_pip_CI.viewport_state_CI;
		pipeline_CI.pVertexInputState = &local_pip_CI.vertex_input_state_CI;
		pipeline_CI.stageCount = static_cast<uint32_t>(local_pip_CI.shader_stage_CI.size());
		pipeline_CI.pStages = local_pip_CI.shader_stage_CI.data();

		pipeline_CI.pTessellationState = nullptr;
		pipeline_CI.pNext = &local_pip_CI.pipeline_rendering_CI;
		pipeline_CI.layout = pipeline_layout;

		VkPipeline pipeline;
		VK_CHECK_RESULT(
			vkCreateGraphicsPipelines(device_manager.GetLogicalDevice(),
				nullptr,
				1,
				&pipeline_CI,
				nullptr,
				&pipeline));

		return std::pair{ std::make_unique<PipelineWrapper>(device_manager,pipeline),std::make_unique<PipelineLayoutWrapper>(device_manager,pipeline_layout) };
	}



	VkPipelineLayout VkPipelineBuilder::BuildPipelineLayout(const GFXPipelineCI& local_pip_CI) const
	{
		VkPipelineLayoutCreateInfo pipe_layout_CI
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = Vk::NoneFlag,
			.setLayoutCount = local_pip_CI.set_layouts.size(),
			.pSetLayouts = local_pip_CI.set_layouts.data(),

			.pushConstantRangeCount = local_pip_CI.push_constant_ranges.size(),
			.pPushConstantRanges = local_pip_CI.push_constant_ranges.data()
		};

		VkPipelineLayout tmp_layout;
		vkCreatePipelineLayout(device_manager.GetLogicalDevice(), &pipe_layout_CI, VK_NULL_HANDLE, &tmp_layout);
		return tmp_layout;
	}

}
