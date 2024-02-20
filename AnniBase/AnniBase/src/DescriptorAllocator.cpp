#include "DescriptorAllocator.h"

namespace Anni
{
	DescriptorSetAllocatorGrowable::DescriptorSetAllocatorGrowable(DeviceManager& device_manager_) :
		device_manager(device_manager_)
	{
		ratios = std::vector<PoolSizeRatio>
		{
			PoolSizeRatio{vk::DescriptorType::eSampler                  ,1.0},
			PoolSizeRatio{vk::DescriptorType::eCombinedImageSampler     ,5.0},
			PoolSizeRatio{vk::DescriptorType::eSampledImage             ,1.0},
			PoolSizeRatio{vk::DescriptorType::eStorageImage             ,1.0},
			PoolSizeRatio{vk::DescriptorType::eUniformTexelBuffer       ,1.0},
			PoolSizeRatio{vk::DescriptorType::eStorageTexelBuffer       ,1.0},
			PoolSizeRatio{vk::DescriptorType::eUniformBuffer            ,1.0},
			PoolSizeRatio{vk::DescriptorType::eStorageBuffer            ,1.0},
			PoolSizeRatio{vk::DescriptorType::eUniformBufferDynamic     ,1.0},
			PoolSizeRatio{vk::DescriptorType::eStorageBufferDynamic     ,1.0},
			PoolSizeRatio{vk::DescriptorType::eInputAttachment          ,1.0},
			PoolSizeRatio{vk::DescriptorType::eInlineUniformBlock       ,1.0},
			PoolSizeRatio{vk::DescriptorType::eInlineUniformBlockEXT    ,1.0},
			PoolSizeRatio{vk::DescriptorType::eAccelerationStructureKHR ,1.0},
			PoolSizeRatio{vk::DescriptorType::eAccelerationStructureNV  ,1.0},
			PoolSizeRatio{vk::DescriptorType::eMutableVALVE             ,1.0},
			PoolSizeRatio{vk::DescriptorType::eSampleWeightImageQCOM    ,1.0},
			PoolSizeRatio{vk::DescriptorType::eBlockMatchImageQCOM      ,1.0},
			PoolSizeRatio{vk::DescriptorType::eMutableEXT               ,1.0},
		};

		sets_per_pool = 32;
		vk::UniqueDescriptorPool newPool = create_pool(sets_per_pool, ratios);
		sets_per_pool = static_cast<uint32_t>(sets_per_pool * 1.5); //grow it next allocation
		ready_pools.push_back(std::move(newPool));

	}


	DescriptorSetAllocatorGrowable::DescriptorSetAllocatorGrowable(DeviceManager& device_manager_, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
		:
		device_manager(device_manager_)
	{
		ratios.clear();
		for (auto r : poolRatios)
		{
			ratios.push_back(r);
		}

		vk::UniqueDescriptorPool newPool = create_pool(initialSets, poolRatios);

		sets_per_pool = static_cast<uint32_t>(initialSets * 1.5); //grow it next allocation
		ready_pools.push_back(std::move(newPool));

	}


	//void DescriptorSetAllocatorGrowable::Init(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
	//{
	//	ratios.clear();
	//	for (auto r : poolRatios)
	//	{
	//		ratios.push_back(r);
	//	}

	//	VkDescriptorPool newPool = create_pool(initialSets, poolRatios);

	//	sets_per_pool = initialSets * 1.5; //grow it next allocation
	//	ready_pools.push_back(newPool);

	//}

	void DescriptorSetAllocatorGrowable::ClearPools()
	{
		for (auto& p : ready_pools)
		{
			device_manager.GetLogicalDevice().resetDescriptorPool(p.get());
		}
		for (auto& p : full_pools)
		{
			device_manager.GetLogicalDevice().resetDescriptorPool(p.get());
			ready_pools.push_back(std::move(p));
		}
		full_pools.clear();
	}

	void DescriptorSetAllocatorGrowable::DestroyPools()
	{
		ready_pools.clear();
		full_pools.clear();
	}


	vk::DescriptorSet DescriptorSetAllocatorGrowable::Allocate(vk::DescriptorSetLayout layout)
	{
		VkDescriptorSetLayout temp_layout = layout;

		//get or create a pool to allocate from
		vk::UniqueDescriptorPool poolToUse = get_pool();

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = poolToUse.get();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &temp_layout;

		VkDescriptorSet ds;
		VkResult result = vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfo, &ds);

		//allocation failed. Try again
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{
			full_pools.push_back(std::move(poolToUse));

			poolToUse = get_pool();
			allocInfo.descriptorPool = poolToUse.get();

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfo, &ds));
		}
		ready_pools.push_back(std::move(poolToUse));
		return ds;
	}


	vk::UniqueDescriptorPool DescriptorSetAllocatorGrowable::get_pool()
	{
		vk::UniqueDescriptorPool newPool;
		if (ready_pools.size() != 0)
		{
			newPool = std::move(ready_pools.back());
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

	vk::UniqueDescriptorPool DescriptorSetAllocatorGrowable::create_pool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios)
	{

		//比如 imge descriptor ratio是5.0，并且set count = 6，那么最后imge descriptor就是30个

		std::vector<vk::DescriptorPoolSize> poolSizes;
		assert(!poolRatios.empty());

		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(
				vk::DescriptorPoolSize(
					ratio.type,
					uint32_t(ratio.ratio * setCount)
				));
		}

		vk::DescriptorPoolCreateInfo pool_info{};
		pool_info.setPoolSizes(poolSizes);
		pool_info.setMaxSets(setCount);

		return device_manager.GetLogicalDevice().createDescriptorPoolUnique(pool_info);
	}

}
