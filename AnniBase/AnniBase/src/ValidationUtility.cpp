#include "ValidationUtility.h"

namespace Anni::ValidationUtility
{
	bool CheckIfRequiredInstanceLayersSupported()
	{
		//֮ǰ�İ汾��device��ĳ��gpu�� layer��instance����������layer�����𣬲����Ѿ�������ô���֣�ȫ������instance layer
		//�������п��õ�LAYER
		//*************************************************************
		std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();

		/*  ��ȡ���п����õ�layer
			VK_LAYER_NV_optimus
			VK_LAYER_NV_nsight
			VK_LAYER_RENDERDOC_Capture
			VK_LAYER_NV_nomad_release_public_2021_1_1
			VK_LAYER_NV_GPU_Trace_release_public_2021_1_1
			VK_LAYER_VALVE_steam_overlay
			VK_LAYER_VALVE_steam_fossilize
			VK_LAYER_LUNARG_api_dump
			VK_LAYER_LUNARG_device_simulation
			VK_LAYER_LUNARG_gfxreconstruct
			VK_LAYER_KHRONOS_synchronization2
			VK_LAYER_KHRONOS_validation
			VK_LAYER_LUNARG_monitor
			VK_LAYER_LUNARG_screenshot
		*/
		//*************************************************************

		//������Ҫ�õ���INSTANCE LAYER��������Ҫ���õ����ڲ��ڿ����б��С�ֻҪ��һ�������þͷ���false
		//*************************************************************
		std::vector<const char*> required_instance_layer;

		if (VALIDATION_LAYERS_ENABLED)
		{
			required_instance_layer.insert(required_instance_layer.end(), required_validation_instance_layers.begin(), required_validation_instance_layers.end());
		}

		for (auto layer_name : required_instance_layer)
		{
			bool layer_found = false;
			for (const auto& layer_properties : available_layers)
			{
				if (strcmp(layer_name, layer_properties.layerName) == 0)
				{
					layer_found = true;
					break;
				}
			}
			if (!layer_found)
			{
				return false;
			}
		}
		return true;
		//*************************************************************
	}



	void SetupDebugMessenger(vk::Instance instance, vk::UniqueDebugUtilsMessengerEXT& debug_messenger)
	{
		if (!ValidationUtility::VALIDATION_LAYERS_ENABLED)
		{
			return;
		}

		vk::DebugUtilsMessengerCreateInfoEXT create_info{};
		ValidationUtility::PopulateDebugMessengerCreateInfo(create_info);
		debug_messenger = instance.createDebugUtilsMessengerEXTUnique(create_info);
	}


	void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info.messageSeverity = vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags;
		create_info.messageType = vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags;
		create_info.pfnUserCallback = DebugCallback;
	}



	VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		//std::ofstream file1;
		//file1.open("D:/CS/ComputerGraphics/vulkan/WindowsProject1/log.txt", std::ios::app);

		const std::string message(pCallbackData->pMessage);
		std::string       debugMessage("DEBUG-PRINTF ]");


		if (messageSeverity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT))
		{
			std::cout << "[VALIDATION LAYER]: " << "\n"
				<< "********************************************************************************" << "\n";
			constexpr auto size_one_line = 140;
			for (size_t i = 0; i < message.length(); i += size_one_line)
			{
				std::cout << message.substr(i, size_one_line) << "\n";
			}

			PrintDebugUtilsObjectNameInfo(pCallbackData->objectCount, pCallbackData->pObjects);
			std::cout << "********************************************************************************" << "\n";
		}
		return VK_FALSE;
	}



	VKAPI_ATTR void PrintDebugUtilsObjectNameInfo(const uint32_t num_of_obj, const VkDebugUtilsObjectNameInfoEXT* object_names)
	{
		for (size_t i = 0; i < num_of_obj; i++)
		{
			if ((object_names + i)->pObjectName)
			{
				std::cout << "--------------------------------------------------------------------------------" << std::endl;
				std::cout << (object_names + i)->pObjectName << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;
			}
		}
	}




}        // namespace Anni::ValidationUtility