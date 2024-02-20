#pragma once
#include "Vk.h"
#include "DeviceManager.h"
#include "VkSynObjectBundleBase.h"

//
//namespace Anni
//{
//
//	class FenceWrapper
//	{
//	public:
//		FenceWrapper(DeviceManager& device_manager_, VkFenceCreateFlags create_flags = Vk::NoneFlag)
//			:device_manager(device_manager_)
//		{
//			VkFenceCreateInfo fence_CI{};
//			fence_CI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//			fence_CI.pNext = nullptr;
//			fence_CI.flags = create_flags;
//
//			VK_CHECK_RESULT(vkCreateFence(device_manager.GetLogicalDevice(), &fence_CI, nullptr, &fence));
//		}
//
//		VkFence& GetRawFence()
//		{
//			return fence;
//		}
//
//	private:
//		DeviceManager& device_manager;
//		VkFence fence;
//
//	};
//
//	//class VkFenceBundle : public VkSynObjectBundleBase
//	//{
//	//public:
//	//	VkFenceBundle(DeviceManager& _device_manager, uint32_t _bundle_size, VkSynObjectBundleBase::SyncObjCreateOption option);
//
//	//	VkFenceBundle() = delete;
//
//	//	VkFenceBundle(const VkFenceBundle&) = delete;
//	//	VkFenceBundle& operator=(const VkFenceBundle&) = delete;
//
//	//	VkFenceBundle(VkFenceBundle&&) = delete;
//	//	VkFenceBundle& operator=(VkFenceBundle&&) = delete;
//
//	//	const VkFence& operator[](size_t num) const;
//	//	[[nodiscard]] const VkFence& GetOne(size_t num) const;
//	//	~VkFenceBundle() override;
//
//	//private:
//	//	std::vector<VkFence>   syn_objects_bundle;
//	//	DeviceManager& device_manager;
//	//};
//
//}
