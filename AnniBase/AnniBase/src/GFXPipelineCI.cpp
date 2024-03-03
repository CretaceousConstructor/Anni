
#include "GFXPipelineCI.h"

namespace Anni
{

	GFXPipelineCI::GFXPipelineCI()
	{
		ClearVectors();
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


	void GFXPipelineCI::SetDynamicRenderingAttachmentFormats(std::vector<DynamicRenderingAttachmentFormatInfo> attachment_formats_)
	{
		//TODO: debug this piece of
		SortingDynamicRenderingAttachment(attachment_formats_);
		attachment_formats = std::move(attachment_formats_);
	}

	void GFXPipelineCI::SetPipelineShaderStageCreateInfo(std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_CI_)
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


	void GFXPipelineCI::AddDescriptorSetLayout(const vk::DescriptorSetLayout& set_layout)
	{
		set_layouts.push_back(set_layout);
	}

	void GFXPipelineCI::AddDescriptorSetLayout(const std::vector<vk::DescriptorSetLayout>& set_layouts_)

	{
		set_layouts.insert(set_layouts.end(), set_layouts_.begin(), set_layouts_.end());
	}


	void GFXPipelineCI::AddPushConstantRange(vk::PushConstantRange push_constant_range)
	{
		push_constant_ranges.push_back(push_constant_range);
	}
}
