#pragma once
#include "DeviceManager.h"

namespace Anni
{
	class DescriptorAllocatorGrowable
	{

	public:
		struct PoolSizeRatio
		{
			VkDescriptorType type;
			float ratio;
		};


	public:
		DescriptorAllocatorGrowable(DeviceManager& device_manager_);
		DescriptorAllocatorGrowable(DeviceManager& device_manager_, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		~DescriptorAllocatorGrowable();


		void Init(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		void ClearPools();
		void DestroyPools();
		VkDescriptorSet Allocate(VkDescriptorSetLayout layout);

	private:
		VkDescriptorPool get_pool();
		VkDescriptorPool create_pool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

		std::vector<PoolSizeRatio> ratios;
		std::vector<VkDescriptorPool> full_pools;
		std::vector<VkDescriptorPool> ready_pools;
		uint32_t sets_per_pool{ 32 };
	private:
		DeviceManager& device_manager;

	};

}