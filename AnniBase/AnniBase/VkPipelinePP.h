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
		explicit GFXPipelineCI();
		~GFXPipelineCI() = default;

		GFXPipelineCI(const GFXPipelineCI&) = default;
		GFXPipelineCI(GFXPipelineCI&&) = default;

		GFXPipelineCI& operator=(const GFXPipelineCI&) = delete;
		GFXPipelineCI& operator=(GFXPipelineCI&&) = delete;



	public:
		void SetDynamicRenderingAttachmentFormats(std::vector<DynamicRenderingAttachmentFormatInfo> attachment_formats_);
		void SetPipelineShaderStageCreateInfo(std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_CI_);

		template <typename First, typename... Rest>
		void AddColorBlendAttachments(First&& first, Rest &&...rest);

		template <typename T>
		void AddColorBlendAttachments(T&& cba);

		template <typename First, typename... Rest>
		void AddDynamicStates(First&& first, Rest &&...rest);

		template <typename T>
		void AddDynamicStates(T&& ds);

		void ClearDynamicState();


		void AddDescriptorSetLayout(const vk::DescriptorSetLayout& set_layout);
		void AddDescriptorSetLayout(const std::vector<vk::DescriptorSetLayout>& set_layouts_);
		void AddPushConstantRange(vk::PushConstantRange push_constant_range);


	private:
		void ClearVectors();
		static void SortingDynamicRenderingAttachment(std::vector<DynamicRenderingAttachmentFormatInfo>& attachment_formats_);

	public:
		struct SpecializationInfoPack
		{
			vk::SpecializationInfo  sp_info;
			vk::ShaderStageFlagBits shader_stage;
		};

	public:
		std::vector<SpecializationInfoPack> specialization_infos;

		std::vector<vk::VertexInputBindingDescription>   vertex_input_binding_descriptions;
		std::vector<vk::VertexInputAttributeDescription> vertex_input_attribute_description;

		std::pair<std::vector<vk::Viewport>, std::vector<vk::Rect2D>> view_port_scissor_pair;
		std::vector<vk::PipelineColorBlendAttachmentState>            color_blend_attachments;
		std::vector<vk::DynamicState>                                 dynamic_states;

		std::vector<vk::DynamicState>                                            dynamic_states_enabled;
		std::vector<DynamicRenderingAttachmentFormatInfo>                        attachment_formats;
		std::vector<vk::PipelineShaderStageCreateInfo>                           shader_stage_CI;
		std::vector<vk::DescriptorSetLayout>                                     set_layouts;
		std::vector<vk::PushConstantRange>                                       push_constant_ranges;


	public:
		vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
		vk::PipelineRasterizationStateCreateInfo rasterization_state_CI{};
		vk::PipelineDepthStencilStateCreateInfo  depth_stencil_CI{};
		vk::PipelineMultisampleStateCreateInfo   multisample_state_CI{};
		vk::PipelineDynamicStateCreateInfo       dynamic_state_CI{};
		vk::PipelineColorBlendStateCreateInfo    color_blend_state_CI{};
		vk::PipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
		vk::PipelineViewportStateCreateInfo      viewport_state_CI{};
		vk::PipelineLayoutCreateInfo             pipeline_layout_CI{};
		vk::PipelineRenderingCreateInfo          pipeline_rendering_CI{};

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
