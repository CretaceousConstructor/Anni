#include "DescriptorAllocator.h"

namespace Anni
{
	DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(DeviceManager& device_manager_) :
		device_manager(device_manager_)
	{
	}


	DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(DeviceManager& device_manager_, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
		:
		device_manager(device_manager_)
	{

		ratios.clear();
		for (auto r : poolRatios)
		{
			ratios.push_back(r);
		}

		VkDescriptorPool newPool = create_pool(initialSets, poolRatios);

		sets_per_pool = initialSets * 1.5; //grow it next allocation
		ready_pools.push_back(newPool);

	}
	DescriptorAllocatorGrowable::~DescriptorAllocatorGrowable()
	{


		for (auto p : ready_pools)
		{
			vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), p, VK_NULL_HANDLE);
		}
		for (auto p : full_pools)
		{
			vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), p, VK_NULL_HANDLE);
		}


	}
	void DescriptorAllocatorGrowable::Init(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
	{
		ratios.clear();
		for (auto r : poolRatios)
		{
			ratios.push_back(r);
		}

		VkDescriptorPool newPool = create_pool(initialSets, poolRatios);

		sets_per_pool = initialSets * 1.5; //grow it next allocation
		ready_pools.push_back(newPool);

	}

	void DescriptorAllocatorGrowable::ClearPools()
	{
		for (auto p : ready_pools)
		{
			vkResetDescriptorPool(device_manager.GetLogicalDevice(), p, Vk::NoneFlag);
		}
		for (auto p : full_pools)
		{
			vkResetDescriptorPool(device_manager.GetLogicalDevice(), p, Vk::NoneFlag);
			ready_pools.push_back(p);
		}
		full_pools.clear();
	}

	void DescriptorAllocatorGrowable::DestroyPools()
	{
		for (auto p : ready_pools)
		{
			vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), p, VK_NULL_HANDLE);
		}
		ready_pools.clear();
		for (auto p : full_pools)
		{
			vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), p, VK_NULL_HANDLE);
		}
		full_pools.clear();
	}


	VkDescriptorSet DescriptorAllocatorGrowable::Allocate(VkDescriptorSetLayout layout)
	{
		//get or create a pool to allocate from
		VkDescriptorPool poolToUse = get_pool();

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = poolToUse;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		VkResult result = vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfo, &ds);

		//allocation failed. Try again
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{
			full_pools.push_back(poolToUse);

			poolToUse = get_pool();
			allocInfo.descriptorPool = poolToUse;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfo, &ds));
		}
		ready_pools.push_back(poolToUse);
		return ds;
	}


	VkDescriptorPool DescriptorAllocatorGrowable::get_pool()
	{
		VkDescriptorPool newPool;
		if (ready_pools.size() != 0)
		{
			newPool = ready_pools.back();
			ready_pools.pop_back();
		}
		else
		{
			//need to create a new pool
			newPool = create_pool(sets_per_pool, ratios);

			sets_per_pool = sets_per_pool * 1.5;
			if (sets_per_pool > 4092)
			{
				sets_per_pool = 4092;
			}
		}
		return newPool;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::create_pool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios)
	{

		//比如 imge descriptor ratio是5.0，并且set count = 6，那么最后imge descriptor就是30个

		std::vector<VkDescriptorPoolSize> poolSizes;
		assert(!poolRatios.empty());

		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(
				VkDescriptorPoolSize{
				.type = ratio.type,
				.descriptorCount = uint32_t(ratio.ratio * setCount)
				});
		}

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = 0;
		pool_info.maxSets = setCount;
		pool_info.poolSizeCount = (uint32_t)poolSizes.size();
		pool_info.pPoolSizes = poolSizes.data();

		VkDescriptorPool newPool;
		vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &pool_info, nullptr, &newPool);
		return newPool;
	}

}
