#pragma once
#include "AnniVkHeader.h"
#include "AnniMath.h"




namespace Anni
{

	enum class MaterialPassType :uint8_t
	{
		MainColor,
		Transparent,
		Other
	};

	struct GPUDrawPushConstants
	{
		glm::mat4 worldMatrix;
		vk::DeviceAddress vertexBuffer;
		static constexpr vk::ShaderStageFlags UseStages{ vk::ShaderStageFlagBits::eVertex };
	};


	//struct MaterialPipeline
	//{
	//	MaterialPipeline() = default;
	//	~MaterialPipeline() = default;

	//	MaterialPipeline(const MaterialPipeline&) = delete;
	//	MaterialPipeline& operator=(const MaterialPipeline&) = delete;

	//	MaterialPipeline(MaterialPipeline&&) = delete;
	//	MaterialPipeline& operator=(MaterialPipeline&&) = delete;

	//	vk::UniquePipeline  pipeline;
	//	vk::UniquePipelineLayout pipeline_layout;
	//};

	struct MaterialInstance   //材质实例，其中pipeline和pipelinelayout是从观察指针pipeline得到的
	{
		//MaterialPipeline* pipeline;
		vk::DescriptorSet materialSet;
		MaterialPassType passType;
	};









}