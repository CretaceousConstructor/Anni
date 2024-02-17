#include "VkPipelinePP.h"
namespace Anni
{

	GFXPipelineCI::GFXPipelineCI()
	{
		ClearVectors();
		InitStype();
	}

	void GFXPipelineCI::ClearVectors()
	{
		//INIT DEFAULT STATE
		shader_stage_CI.clear();

		specialization_infos.clear();

		vertex_input_binding_descriptions.clear();
		vertex_input_attribute_description.clear();

		view_port_scissor_pair.first.clear();
		view_port_scissor_pair.second.clear();

		color_blend_attachments.clear();
		dynamic_states.clear();
	}

	void GFXPipelineCI::InitStype()
	{
		input_assembly_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		rasterization_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		depth_stencil_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		multisample_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		dynamic_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		color_blend_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		vertex_input_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		viewport_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipeline_layout_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_rendering_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

	}

	void GFXPipelineCI::SetDynamicRenderingAttachmentFormats(std::vector<DynamicRenderingAttachmentFormatInfo> attachment_formats_)
	{
		//TODO: debug this piece of
		SortingDynamicRenderingAttachment(attachment_formats_);
		attachment_formats = std::move(attachment_formats_);
	}

	void GFXPipelineCI::SetPipelineShaderStageCreateInfo(std::vector<VkPipelineShaderStageCreateInfo> shader_stage_CI_)
	{
		shader_stage_CI = std::move(shader_stage_CI_);
	}

	void GFXPipelineCI::ClearDynamicState()
	{
		dynamic_states_enabled.clear();
	}

	void GFXPipelineCI::SortingDynamicRenderingAttachment(std::vector<DynamicRenderingAttachmentFormatInfo>& attachment_formats_)
	{
		std::ranges::sort(attachment_formats_,
			[](const DynamicRenderingAttachmentFormatInfo& lhs,
				const DynamicRenderingAttachmentFormatInfo& rhs) -> bool
			{
				assert(lhs.index != rhs.index);
				return lhs.index < rhs.index;
			});
	}

}
