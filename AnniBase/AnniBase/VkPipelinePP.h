#pragma once
#include "Vk.h"
#include "VkRsrcUsageInfo.h"
#include "GraphicsComponent.h"
#include "VkShaderFactory.h"
#include <algorithm>
#include <vector>


namespace Anni
{

	class GFXPipelineCI
	{
		friend class IPipelineCIBuilder;
		friend class VkPipelineBuilder;
	public:
		~GFXPipelineCI() = default;

		GFXPipelineCI(const GFXPipelineCI&) = default;
		GFXPipelineCI(GFXPipelineCI&&) = default;

		GFXPipelineCI& operator=(const GFXPipelineCI&) = delete;
		GFXPipelineCI& operator=(GFXPipelineCI&&) = delete;



	public:
		void SetDynamicRenderingAttachmentFormats(std::vector<DynamicRenderingAttachmentFormatInfo> attachment_formats_);
		void SetPipelineShaderStageCreateInfo(std::vector<VkPipelineShaderStageCreateInfo> shader_stage_CI_);

		template <typename First, typename... Rest>
		void AddColorBlendAttachments(First&& first, Rest &&...rest);

		template <typename T>
		void AddColorBlendAttachments(T&& cba);

		template <typename First, typename... Rest>
		void AddDynamicStates(First&& first, Rest &&...rest);

		template <typename T>
		void AddDynamicStates(T&& ds);

		void ClearDynamicState();


		void AddDescriptorSetLayout(VkDescriptorSetLayout set_layout)
		{
			set_layouts.push_back(set_layout);
		}


		void AddDescriptorSetLayout(const std::vector<VkDescriptorSetLayout> &set_layouts_)
		{
			set_layouts.insert(set_layouts.end(),set_layouts_.begin(),set_layouts_.end());
		}

		void AddPushConstantRange(VkPushConstantRange push_constant_range)
		{
			push_constant_ranges.push_back(push_constant_range);
		}


	private:
		explicit GFXPipelineCI();
		void ClearVectors();
		void InitStype();
		static void SortingDynamicRenderingAttachment(std::vector<DynamicRenderingAttachmentFormatInfo>& attachment_formats_);

	public:
		struct VkSpecializationInfoPack
		{
			VkSpecializationInfo  sp_info;
			VkShaderStageFlagBits shader_stage;
		};

	public:
		std::vector<VkSpecializationInfoPack> specialization_infos;

		std::vector<VkVertexInputBindingDescription>   vertex_input_binding_descriptions;
		std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_description;

		std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_port_scissor_pair;
		std::vector<VkPipelineColorBlendAttachmentState>          color_blend_attachments;
		std::vector<VkDynamicState>                               dynamic_states;

		std::vector<VkDynamicState>                                              dynamic_states_enabled;
		std::vector<DynamicRenderingAttachmentFormatInfo>                        attachment_formats;
		std::vector<VkPipelineShaderStageCreateInfo>                             shader_stage_CI;
		std::vector<VkDescriptorSetLayout>                                       set_layouts;
		std::vector<VkPushConstantRange>                                         push_constant_ranges;


	public:
		VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
		VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
		VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};
		VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
		VkPipelineDynamicStateCreateInfo dynamic_state_CI{};
		VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};
		VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
		VkPipelineViewportStateCreateInfo      viewport_state_CI{};
		VkPipelineLayoutCreateInfo pipeline_layout_CI{};
		VkPipelineRenderingCreateInfo pipeline_rendering_CI{};

	};

	template <typename First, typename ... Rest>
	void GFXPipelineCI::AddColorBlendAttachments(First&& first, Rest &&... rest)
	{
		color_blend_attachments.clear();
		color_blend_attachments.push_back(first);
		AddColorBlendAttachments(std::forward<Rest>(rest)...);

	}

	template <typename T>
	void GFXPipelineCI::AddColorBlendAttachments(T&& cba)
	{
		color_blend_attachments.push_back(cba);
	}



	template <typename First, typename... Rest>
	void GFXPipelineCI::AddDynamicStates(First&& first, Rest &&...rest)
	{
		dynamic_states_enabled.clear();
		dynamic_states_enabled.push_back(first);
		AddDynamicStates(std::forward<Rest>(rest)...);
	}

	template <typename T>
	void GFXPipelineCI::AddDynamicStates(T&& ds)
	{
		dynamic_states_enabled.push_back(ds);
		//dynamic_state_CI.pDynamicStates    = dynamic_states_enabled.data();
		//dynamic_state_CI.dynamicStateCount = static_cast<uint32_t>(dynamic_states_enabled.size());

		return;
	}
}
