#include "PipelineBuilder.h"
namespace Anni
{

	PipelineBuilder::PipelineBuilder(DeviceManager& device_manager_, ShaderFactory& shader_factory_) :
		device_manager(device_manager_)
	{
	}

	/// <summary>
	/// 填写所有CI，然后创建 
	/// </summary>
	/// <param name="local_pip_CI"></param>
	/// <returns></returns>
	std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> PipelineBuilder::BuildPipeline(GFXPipelineCI local_pip_CI) const
	{
		//******************error detecting******************

		//COLOR BLEND
		local_pip_CI.color_blend_state_CI.setAttachments(local_pip_CI.color_blend_attachments);
		//VERTEX INPUT
		local_pip_CI.vertex_input_state_CI.setVertexBindingDescriptions(local_pip_CI.vertex_input_binding_descriptions);
		local_pip_CI.vertex_input_state_CI.setVertexAttributeDescriptions(local_pip_CI.vertex_input_attribute_description);

		//DYNAMIC RENDERING ATTACHMENT FORMATS 
		std::vector<vk::Format> color_attachments;

		for (const auto& format : local_pip_CI.attachment_formats)
		{
			if (AttachmentType::ColorAttachment == format.attach_type)
			{
				color_attachments.push_back(format.format);
			}

			if (AttachmentType::DepthAttachment == format.attach_type)
			{
				local_pip_CI.pipeline_rendering_CI.depthAttachmentFormat = format.format;
				local_pip_CI.pipeline_rendering_CI.stencilAttachmentFormat = vk::Format::eUndefined;
			}

			if (AttachmentType::DepthStencilAttachment == format.attach_type)
			{
				local_pip_CI.pipeline_rendering_CI.depthAttachmentFormat = format.format;
				local_pip_CI.pipeline_rendering_CI.stencilAttachmentFormat = format.format;
			}
		}

		local_pip_CI.pipeline_rendering_CI.setColorAttachmentFormats(color_attachments);
		local_pip_CI.pipeline_rendering_CI.viewMask = 0;


		//COLOR BLEND AND COLOR ATTACH MUST HAVE THE SAME NUMBER
		if (local_pip_CI.pipeline_rendering_CI.colorAttachmentCount != local_pip_CI.color_blend_state_CI.attachmentCount && local_pip_CI.color_blend_state_CI.attachmentCount == 1)
		{
			const auto first_color_blend_attachment = local_pip_CI.color_blend_attachments.front();
			local_pip_CI.color_blend_attachments.resize(local_pip_CI.pipeline_rendering_CI.colorAttachmentCount, first_color_blend_attachment);

			local_pip_CI.color_blend_state_CI.setAttachments(local_pip_CI.color_blend_attachments);
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
		local_pip_CI.dynamic_state_CI.setDynamicStates(local_pip_CI.dynamic_states);


		//VIEWPORT AND SCISSOR
		local_pip_CI.viewport_state_CI.setViewports(local_pip_CI.view_port_scissor_pair.first);
		local_pip_CI.viewport_state_CI.setScissors(local_pip_CI.view_port_scissor_pair.second);


		//PIPLINE CI (use dynamic rendering)
		vk::GraphicsPipelineCreateInfo pipeline_CI{};

		//pipeline_CI.renderPass = VK_NULL_HANDLE;        //use dynamic rendering
		//pipeline_CI.subpass = 0;                        //use dynamic rendering

		pipeline_CI.basePipelineIndex = -1;
		pipeline_CI.basePipelineHandle = VK_NULL_HANDLE;


		//PIPELINE LAYOUT
		vk::UniquePipelineLayout pipeline_layout = BuildPipelineLayout(local_pip_CI);

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
		pipeline_CI.layout = pipeline_layout.get();

		vk::ResultValue<vk::UniquePipeline> temp_pipline = device_manager.GetLogicalDevice().createGraphicsPipelineUnique(VK_NULL_HANDLE, pipeline_CI);
		vk::resultCheck(temp_pipline.result, "pipeline failed to create!");

		return std::pair{
			std::move(temp_pipline.value),
			std::move(pipeline_layout)
		};
	}





	vk::UniquePipelineLayout PipelineBuilder::BuildPipelineLayout(const GFXPipelineCI& local_pip_CI) const
	{
		vk::PipelineLayoutCreateInfo pipe_layout_CI{};
		pipe_layout_CI.setSetLayouts(local_pip_CI.set_layouts);
		pipe_layout_CI.setPushConstantRanges(local_pip_CI.push_constant_ranges);

		return device_manager.GetLogicalDevice().createPipelineLayoutUnique(pipe_layout_CI);
	}

}
