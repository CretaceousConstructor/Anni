#pragma once
#include "Buffer.h"
#include "VkTexture.h"
#include "DesSetLayoutManager.h"
#include "VkPipelineBuilder.h"
#include "DescriptorAllocator.h"
#include "Mate.h"

namespace Anni
{

	//提供pipeline（根据需求变动）， 提供descriptor set layout（可以提前构建）
	//重新命名为GLTFMetallicRoughnessProducer

	//TODO: Pipeline pre-build

	class GLTFMetallicRoughnessProducer
	{
	public:
		GLTFMetallicRoughnessProducer(
			DeviceManager& device_manager_,
			DescriptorLayoutManager& descriptor_layout_manager_,
			VkPipelineBuilder& pipeline_builder_
		);

	private:
		DeviceManager& device_manager;
		DescriptorLayoutManager& descriptor_layout_manager;
		VkPipelineBuilder& pipeline_builder;
	private:
		//MaterialPipeline transparentPipeline;
		//MaterialPipeline        opaquePipeline;

		vk::DescriptorSetLayout materialLayout;
		vk::PushConstantRange   matrixRange;
	public:
		std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> BuildPipelines(GFXPipelineCI gfx_pip_CI);
		MaterialInstance Instantiate(MaterialPassType pass_t, DescriptorSetAllocatorGrowable& descriptorAllocator);

	};

}

