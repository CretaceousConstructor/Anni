#pragma once
#include "DeviceManager.h"

namespace Anni
{
	class DescriptorSetAllocatorGrowable
	{

	public:
		struct PoolSizeRatio
		{
			vk::DescriptorType type;
			float ratio;
		};


	public:
		//DescriptorSetAllocatorGrowable(DeviceManager& device_manager_);
		DescriptorSetAllocatorGrowable(DeviceManager& device_manager_, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		DescriptorSetAllocatorGrowable(DeviceManager& device_manager_);
		~DescriptorSetAllocatorGrowable() = default;


		//void Init(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		void ClearPools();
		void DestroyPools();
		vk::DescriptorSet Allocate(vk::DescriptorSetLayout layout);

	private:
		vk::UniqueDescriptorPool get_pool();
		vk::UniqueDescriptorPool create_pool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

		std::vector<PoolSizeRatio> ratios;
		std::vector<vk::UniqueDescriptorPool> full_pools;
		std::vector<vk::UniqueDescriptorPool> ready_pools;
		uint32_t sets_per_pool;
	private:
		DeviceManager& device_manager;

	};

}