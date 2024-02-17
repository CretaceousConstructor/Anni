#include "TimelineSemPoolUnsafe.h"

namespace Anni
{
	VkTimelineSemPoolUnsafe::VkTimelineSemPoolUnsafe(DeviceManager& device_manager_) :
		device_manager(device_manager_)
	{
	}

	std::shared_ptr<TimelineSemWrapper> VkTimelineSemPoolUnsafe::AcquireTimelineSem()
	{
		if (!pool.empty())
		{
			auto result_timeline_sem = pool.front();
			pool.pop_front();

			return std::shared_ptr<TimelineSemWrapper>(result_timeline_sem, [this](TimelineSemWrapper* ptr)
				{
					this->ReleaseObject(ptr);
				});
		}
		else
		{
			auto result_timeline_sem = new TimelineSemWrapper(device_manager);
			return std::shared_ptr<TimelineSemWrapper>(result_timeline_sem, [this](TimelineSemWrapper* ptr)
				{
					this->ReleaseObject(ptr);
				});
		}
	}

	void VkTimelineSemPoolUnsafe::ReleaseObject(TimelineSemWrapper* obj)
	{
		// Optionally reset the object state
		if (pool.size() < max_capacity)
		{
			const uint64_t cur_val = obj->ReturnState();
			obj->GetLastValue() = cur_val;
			pool.push_back(obj);
		}
		else
		{
			delete obj;
		}
	}

}        // namespace Anni
