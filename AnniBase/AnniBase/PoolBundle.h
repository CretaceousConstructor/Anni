#pragma once

#include "DeviceManager.h"
#include "TimelineSemPoolUnsafe.h"

namespace Anni
{

	class PoolBundle
	{
	public:
		PoolBundle(DeviceManager& device_manager_);

	private:
		DeviceManager& device_manager;

	private:
		VkTimelineSemPoolUnsafe timeline_sem_pool;
	};

}        // namespace Anni
