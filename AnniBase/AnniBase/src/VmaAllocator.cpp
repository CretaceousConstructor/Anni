#include "VmaAllocator.h"

Anni::VmaAllocatorWrapper::VmaAllocatorWrapper(InstanceWrapper& instance_wrapper_, DeviceManager& device_manager_):
	instance_wrapper(instance_wrapper_),
	device_manager(device_manager_)
{

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	allocatorInfo.vulkanApiVersion = ANNI_VK_API_VERSION;
	allocatorInfo.physicalDevice = device_manager.GetPhysicalDevice();
	allocatorInfo.device = device_manager.GetLogicalDevice();
	allocatorInfo.instance = instance_wrapper.GetInstanceReFac();

	vmaCreateAllocator(&allocatorInfo, &allocator);
}

Anni::VmaAllocatorWrapper::~VmaAllocatorWrapper()
{
	vmaDestroyAllocator(allocator);
}

VmaAllocator& Anni::VmaAllocatorWrapper::GetRaw()
{
	return allocator;
}
