#pragma once

#include "TimelineSemWrapper.h"
#include <deque>

namespace Anni
{

	class VkTimelineSemPoolUnsafe
	{
	public:
		static constexpr size_t max_capacity{ 64 };
		VkTimelineSemPoolUnsafe(DeviceManager& device_manager_);
		std::shared_ptr<TimelineSemWrapper> AcquireTimelineSem();
		void                                ReleaseObject(TimelineSemWrapper* obj);
	private:
		std::deque<TimelineSemWrapper*> pool;
	private:
		DeviceManager& device_manager;
	};




}        // namespace Anni
