#pragma once
#include "GraphicsComponent.h"
#include "VkPipelinePP.h"
#include "VkShaderFactory.h"
#include <memory>
#include <ranges>

namespace Anni
{


	//class PipelineWrapper
	//{
	//public:

	//	PipelineWrapper(
	//		DeviceManager& device_manager_,
	//		VkPipeline   pipeline_) :
	//		device_manager(device_manager_),
	//		pipeline(pipeline_)
	//	{

	//	}

	//	VkPipeline GetRaw()
	//	{
	//		return pipeline;

	//	}


	//	PipelineWrapper() = delete;
	//	PipelineWrapper(const PipelineWrapper&) = delete;
	//	PipelineWrapper(PipelineWrapper&&) = delete;
	//	PipelineWrapper& operator=(const PipelineWrapper&) = delete;
	//	PipelineWrapper& operator=(PipelineWrapper&&) = delete;
	//	~PipelineWrapper()
	//	{
	//		vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline, VK_NULL_HANDLE);
	//	}

	//private:

	//	DeviceManager& device_manager;
	//	VkPipeline   pipeline;
	//};
	//// End of class PipelineWrapper


	//class PipelineLayoutWrapper
	//{
	//public:

	//	PipelineLayoutWrapper(
	//		DeviceManager& device_manager_,
	//		VkPipelineLayout   pipeline_layout_) :
	//		device_manager(device_manager_),
	//		pipeline_layout(pipeline_layout_)
	//	{

	//	}

	//	VkPipelineLayout GetRaw()
	//	{
	//		return pipeline_layout;

	//	}


	//	PipelineLayoutWrapper() = delete;
	//	PipelineLayoutWrapper(const PipelineLayoutWrapper&) = delete;
	//	PipelineLayoutWrapper(PipelineLayoutWrapper&&) = delete;
	//	PipelineLayoutWrapper& operator=(const PipelineLayoutWrapper&) = delete;
	//	PipelineLayoutWrapper& operator=(PipelineLayoutWrapper&&) = delete;
	//	~PipelineLayoutWrapper()
	//	{
	//		vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipeline_layout, VK_NULL_HANDLE);
	//	}


	//private:
	//	DeviceManager& device_manager;
	//	VkPipelineLayout   pipeline_layout;

	//};
	// End of class PipelineLayoutWrapper





	class VkPipelineBuilder
	{
	public:
		VkPipelineBuilder(DeviceManager& device_manager_, VkShaderFactory& shader_factory_);
		~VkPipelineBuilder() = default;

		VkPipelineBuilder() = delete;
		VkPipelineBuilder(const VkPipelineBuilder&) = delete;
		VkPipelineBuilder& operator=(const VkPipelineBuilder&) = delete;
		VkPipelineBuilder(VkPipelineBuilder&&) = delete;
		VkPipelineBuilder& operator=(VkPipelineBuilder&&) = delete;

	public:
		//[[nodiscard]] std::pair<std::unique_ptr<PipelineWrapper>, std::unique_ptr<PipelineLayoutWrapper>> BuildPipeline(GFXPipelineCI pip_CI) const;
		[[nodiscard]] std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> BuildPipeline(GFXPipelineCI pip_CI) const;


	private:
		vk::UniquePipelineLayout BuildPipelineLayout(const GFXPipelineCI& pip_CI) const;
	private:
		DeviceManager& device_manager;

	};







}