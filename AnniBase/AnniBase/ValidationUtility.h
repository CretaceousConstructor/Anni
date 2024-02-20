#pragma once

#include "AnniVkHeader.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>


namespace Anni::ValidationUtility
{
	//let vulkan configurator take charge
	constexpr bool VALIDATION_LAYERS_ENABLED = VK_FALSE;

	//��Ҫ�õ��ĺ�VALIDATION��ص�INSTANCE LAYER
	static const std::array required_validation_instance_layers =
	{
		   "VK_LAYER_KHRONOS_validation",
	};

	//��Ҫ�õ�������INSTANCE LAYER
	//static const std::array  required_other_instance_layers{
	//     bla bla bla
	//     "VK_LAYER_RENDERDOC_Capture"
	//};

	//��Щ��������Ҫ���������õ���validationFeatures,
	const std::vector<vk::ValidationFeatureEnableEXT> enabled_validation_features{
		vk::ValidationFeatureEnableEXT::eDebugPrintf,
	};

	const std::vector<vk::ValidationFeatureDisableEXT> disabled_validation_features{

		vk::ValidationFeatureDisableEXT::eThreadSafety,
		vk::ValidationFeatureDisableEXT::eApiParameters,
		vk::ValidationFeatureDisableEXT::eObjectLifetimes,
		//eCoreChecks
	};


	bool CheckIfRequiredInstanceLayersSupported();
	void SetupDebugMessenger(vk::Instance instance, vk::UniqueDebugUtilsMessengerEXT& debug_messenger);
	void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info);

	VKAPI_ATTR void PrintDebugUtilsObjectNameInfo(const uint32_t num_of_obj, const VkDebugUtilsObjectNameInfoEXT* object_names);


	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

};        // namespace Anni::ValidationUtility
