#pragma once
#include "InstanceWrapper.h"
#include "DeviceManager.h"
#include <vma/vk_mem_alloc.h>

namespace Anni
{

	class VmaAllocatorWrapper
	{
	public:
		VmaAllocatorWrapper(InstanceWrapper& instance_wrapper_, DeviceManager& device_manager_) :
			instance_wrapper(instance_wrapper_),
			device_manager(device_manager_)
		{

			VmaAllocatorCreateInfo allocatorInfo = {};

			allocatorInfo.vulkanApiVersion = ANNI_VK_API_VERSION;
			allocatorInfo.physicalDevice = device_manager.GetPhysicalDevice();
			allocatorInfo.device = device_manager.GetLogicalDevice();
			allocatorInfo.instance = instance_wrapper.GetInstanceReFac();

			vmaCreateAllocator(&allocatorInfo, &allocator);
		}

		VmaAllocatorWrapper(const VmaAllocatorWrapper&) = delete;
		VmaAllocatorWrapper(VmaAllocatorWrapper&&) = delete;
		VmaAllocatorWrapper& operator=(const VmaAllocatorWrapper&) = delete;
		VmaAllocatorWrapper& operator=(VmaAllocatorWrapper&&) = delete;
		~VmaAllocatorWrapper()
		{
			vmaDestroyAllocator(allocator);
		}

		VmaAllocator& GetRaw()
		{
			return allocator;
		}

	private:
		InstanceWrapper& instance_wrapper;
		DeviceManager& device_manager;
		VmaAllocator allocator;

	};
	// End of class VmaAllocator



}
