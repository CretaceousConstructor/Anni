#pragma once
#include "Vk.h"
#include "DeviceManager.h"
#include "VkSynObjectBundleBase.h"
namespace Anni
{

	class VkEventBundle : public VkSynObjectBundleBase
	{
	public:
		VkEventBundle(DeviceManager& _device_manager, uint32_t _bundle_size);

		VkEventBundle() = delete;
		VkEventBundle(const VkEventBundle&) = delete;
		VkEventBundle& operator=(const VkEventBundle&) = delete;
		VkEventBundle(VkEventBundle&&) = delete;
		VkEventBundle& operator=(VkEventBundle&&) = delete;

		const VkEvent& operator[](size_t num) const;
		[[nodiscard]] const VkEvent& GetOne(size_t num) const;

		~VkEventBundle() override;

	private:
		std::vector<VkEvent> syn_objects_bundle;
		DeviceManager& device_manager;
	};

}