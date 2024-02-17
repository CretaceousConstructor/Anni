#pragma once

//namespace Anni
//{
//struct DescriptorAllocatorGrowableUnsafe
//{
//  public:
//	struct PoolSizeRatio
//	{
//		VkDescriptorType type;
//		float            ratio;
//	};
//
//	DescriptorAllocatorGrowableUnsafe(GraphicsComponent &gfx_) :
//	    device_manager(gfx_.DeviceMan())
//	{
//	}
//
//	~DescriptorAllocatorGrowableUnsafe()
//	{
//	}
//	DescriptorAllocatorGrowableUnsafe(const DescriptorAllocatorGrowableUnsafe &) = delete;
//	DescriptorAllocatorGrowableUnsafe &operator=(const DescriptorAllocatorGrowableUnsafe &) = delete;
//
//	DescriptorAllocatorGrowableUnsafe(DescriptorAllocatorGrowableUnsafe &&) = delete;
//	DescriptorAllocatorGrowableUnsafe &operator=(DescriptorAllocatorGrowableUnsafe &&) = delete;
//
//	void Init(VkDevice device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
//	{
//	}
//
//	void clear_pools(VkDevice device);
//	void destroy_pools(VkDevice device);
//
//	VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
//
//  private:
//	VkDescriptorPool GetPool()
//	{
//		VkDescriptorPool new_pool;
//		if (!ready_pools.empty())
//		{
//			new_pool = ready_pools.back();
//			ready_pools.pop_back();
//		}
//		else
//		{
//			//need to create a new pool
//			new_pool = CreatePool();
//
//			sets_per_pool = sets_per_pool * 1.5;
//			if (sets_per_pool > 4092)
//			{
//				sets_per_pool = 4092;
//			}
//		}
//
//		return new_pool;
//	}
//
//
//
//
//	VkDescriptorPool CreatePool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);
//
//	std::vector<PoolSizeRatio>    ratios;
//	std::vector<VkDescriptorPool> fully_pools;
//	std::vector<VkDescriptorPool> ready_pools;
//	uint32_t                      sets_per_pool;
//
//  private:
//	DeviceManager &device_manager;
//};

//}        // namespace Anni
