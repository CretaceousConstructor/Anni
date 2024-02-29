#pragma once
#include "InstanceWrapper.h"
#include "DeviceManager.h"
#include <vma/vk_mem_alloc.h>

namespace Anni
{

	class VmaAllocatorWrapper
	{
	public:
		VmaAllocatorWrapper(InstanceWrapper& instance_wrapper_, DeviceManager& device_manager_);

		VmaAllocatorWrapper(const VmaAllocatorWrapper&) = delete;
		VmaAllocatorWrapper(VmaAllocatorWrapper&&) = delete;
		VmaAllocatorWrapper& operator=(const VmaAllocatorWrapper&) = delete;
		VmaAllocatorWrapper& operator=(VmaAllocatorWrapper&&) = delete;
		~VmaAllocatorWrapper();

		VmaAllocator& GetRaw();

	private:
		InstanceWrapper& instance_wrapper;
		DeviceManager& device_manager;
		VmaAllocator allocator;

	};
	// End of class VmaAllocator



}
