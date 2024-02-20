#include "DesSetLayoutManager.h"

namespace Anni
{

	vk::DescriptorSetLayoutCreateInfo CI::GetDescriptorSetLayoutCI(const std::vector<vk::DescriptorSetLayoutBinding> bindings, vk::DescriptorSetLayoutCreateFlags flags, const void* pNext)
	{
		//TODO:	use perfect forwarding to check bindings is not a rvalue;
		vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
		descriptor_set_layout_create_info.setBindings(bindings);
		descriptor_set_layout_create_info.pNext = pNext;
		descriptor_set_layout_create_info.flags = flags;
		return descriptor_set_layout_create_info;
	}


	DescriptorLayoutManager::DescriptorLayoutManager(DeviceManager& device_manager_) :
		device_manager(device_manager_)
	{
 

	}






}

