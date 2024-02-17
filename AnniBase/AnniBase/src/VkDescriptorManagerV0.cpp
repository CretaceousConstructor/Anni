#include "VkDescriptorManagerV0.h"
namespace Anni
{
	VkDescriptorManagerV0::VkDescriptorManagerV0(GraphicsComponent& gfx_) :
		device_manager(gfx_.DeviceMan())
	{

	}
	VkDescriptorManagerV0::~VkDescriptorManagerV0()
	{
		for (const auto& pair : desc_set_pools_unsafe)
		{
			for (uint32_t i = 0; i < pair.second; i++)
			{
				vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), pair.first, nullptr);
			}
		}

		for (const auto& pair : desc_set_layouts_unsafe)
		{
			for (uint32_t i = 0; i < pair.second; i++)
			{
				vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), pair.first, nullptr);
			}
		}
	}

	VkDescriptorPool VkDescriptorManagerV0::ProduceDescriptorPoolUnsafe(const VkDescriptorPoolCreateInfo& desc_pool_CI,
		const VkAllocationCallbacks* pAllocator)
	{
		VkDescriptorPool descriptor_pool;
		VK_CHECK_RESULT(vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &desc_pool_CI, pAllocator, &descriptor_pool))

			if (desc_set_pools_unsafe.contains(descriptor_pool))
			{
				++desc_set_pools_unsafe.at(descriptor_pool);
			}
			else
			{
				desc_set_pools_unsafe.insert(std::pair{ descriptor_pool, 1 });
			}

		return descriptor_pool;
	}

	void VkDescriptorManagerV0::DestroyDescriptorPoolUnsafe(VkDescriptorPool desc_pool)
	{
		auto& amount = desc_set_pools_unsafe.at(desc_pool);
		assert(amount != 0);
		if (0 == --amount)
		{
			vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), desc_pool, nullptr);
		}
	}


	//非线程安全的方式创造DescriptorSetLayout
	VkDescriptorSetLayout VkDescriptorManagerV0::ProduceDescriptorSetLayoutUnsafe(const VkDescriptorSetLayoutCreateInfo& desc_set_layout_CI, const VkAllocationCallbacks* pAllocator)
	{
		assert(false);
		//vkdescriptorsetlayout descriptor_set_layout;
		//vk_check_result(vkcreatedescriptorsetlayout(device_manager.getlogicaldevice(), &desc_set_layout_ci, nullptr, &descriptor_set_layout))

		//	if (desc_set_layouts_unsafe.contains(descriptor_set_layout))
		//	{
		//		++desc_set_layouts_unsafe.at(descriptor_set_layout);
		//	}
		//	else
		//	{
		//		desc_set_layouts_unsafe.insert(std::pair{ descriptor_set_layout, 1 });
		//	}

		//return descriptor_set_layout;
	}

	void VkDescriptorManagerV0::DestroyDescriptorSetLayoutUnsafe(VkDescriptorSetLayout desc_set_layout)
	{
		auto& amount = desc_set_layouts_unsafe.at(desc_set_layout);
		assert(amount != 0);
		if (0 == --amount)
		{
			vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_set_layout, nullptr);
		}
	}



}
