#pragma once
#include "Vk.h"
#include "DeviceManager.h"


namespace Anni
{
	class BinarySemWrapper
	{
	public:
		BinarySemWrapper(DeviceManager& device_manager_);

		vk::Semaphore& GetRaw();

	private:
		DeviceManager& device_manager;
		vk::UniqueSemaphore semaphore;

	};

}



//class VkSemaphoreBundle : public VkSynObjectBundleBase
//{
//public:
//	vkCreateSemaphore: parameter pCreateInfo->flags must be 0. The Vulkan spec states: flags must be 0
//	VkSemaphoreBundle(const DeviceManager& _device_manager, uint32_t _bundle_size);
//
//	VkSemaphoreBundle() = delete;
//
//	VkSemaphoreBundle(const VkSemaphoreBundle&) = delete;
//	VkSemaphoreBundle& operator=(const VkSemaphoreBundle&) = delete;
//
//	VkSemaphoreBundle(VkSemaphoreBundle&&) = delete;
//	VkSemaphoreBundle& operator=(VkSemaphoreBundle&&) = delete;
//
//	const VkSemaphore& operator[](size_t num)const;
//	[[nodiscard]] const VkSemaphore& GetOne(size_t num) const;
//
//	~VkSemaphoreBundle() override;
//
//private:
//	std::vector<VkSemaphore> syn_objects_bundle;
//	const DeviceManager& device_manager;
//};
