#pragma once
#include "DeviceManager.h"
namespace Anni
{
	class TimelineSemWrapper
	{
		friend class VkTimelineSemPoolUnsafe;
	public:
		uint64_t       ReturnState();
		vk::Semaphore& GetRaw();

		std::uint64_t& GetLastValue();

		TimelineSemWrapper() = delete;
		TimelineSemWrapper(const TimelineSemWrapper&) = delete;
		TimelineSemWrapper& operator=(const TimelineSemWrapper&) = delete;

		TimelineSemWrapper(TimelineSemWrapper&& rhs) = delete;
		TimelineSemWrapper& operator=(TimelineSemWrapper&&) = delete;

		~TimelineSemWrapper() = default;
		TimelineSemWrapper(DeviceManager& device_manager_, uint64_t inital_val = 0u);

	private:
		DeviceManager& device_manager;
		vk::UniqueSemaphore    time_line_sem;

		std::uint64_t   last_value;

	};

}        // namespace Anni