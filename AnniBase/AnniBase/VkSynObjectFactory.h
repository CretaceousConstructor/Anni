#pragma once

#include "GraphicsComponent.h"
#include "Vk.h"
#include "VkEventBundle.h"
#include "VkFenceBundle.h"
#include "VkSemaphoreBundle.h"
#include <algorithm>
#include <ranges>
#include <unordered_map>
namespace Anni
{
	//class VkSynObjectFactory
	//{
	//public:
	//	VkSynObjectFactory(GraphicsComponent& _gfx);

	//	//vkCreateSemaphore: parameter pCreateInfo->flags must be 0. The Vulkan spec states: flags must be 0
	//	[[nodiscard]] std::shared_ptr<VkSemaphoreBundle> GetSemaphoreBundle(uint32_t _bundle_size) const;
	//	[[nodiscard]] std::shared_ptr<VkEventBundle>     GetEventBundle(uint32_t _bundle_size) const;
	//	[[nodiscard]] std::shared_ptr<VkFenceBundle>     GetFenceBundle(uint32_t _bundle_size, VkSynObjectBundleBase::SyncObjCreateOption option = VkSynObjectBundleBase::Unsignaled) const;

	//private:
	//	GraphicsComponent& gfx;
	//	const DeviceManager& device_manager;
	//	const WindowsSys& window;
	//};



}        // namespace Anni
