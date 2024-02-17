#include "DesSetLayoutManager.h"

namespace Anni
{

	VkDescriptorSetLayoutCreateInfo CI::GetDescriptorSetLayoutCI(const std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags, const void* pNext)
	{
		//TODO:	use perfect forwarding to check bindings is not a rvalue;
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
		descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pBindings = bindings.data();
		descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptor_set_layout_create_info.pNext = pNext;
		descriptor_set_layout_create_info.flags = flags;
		return descriptor_set_layout_create_info;
	}


	DescriptorLayoutManager::DescriptorLayoutManager(DeviceManager& device_manager_) :
		device_manager(device_manager_)
	{


	}

	DescriptorLayoutManager::~DescriptorLayoutManager()
	{
		for (const auto& [_, set_laytout] : mat_deslayout_map)
		{
			vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), set_laytout, VK_NULL_HANDLE);
		}
	}




	VkDescriptorSetLayoutBinding DescVk::GetDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t descriptor_count, const VkSampler* pImmutableSamplers)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding{};
		setLayoutBinding.descriptorType = type;
		setLayoutBinding.stageFlags = stage_flags;
		setLayoutBinding.binding = binding;
		setLayoutBinding.descriptorCount = descriptor_count;
		setLayoutBinding.pImmutableSamplers = pImmutableSamplers;
		return setLayoutBinding;
	}









}

