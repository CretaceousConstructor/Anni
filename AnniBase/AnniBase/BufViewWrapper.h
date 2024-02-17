#pragma once

#include "DeviceManager.h"
#include "AnniVkHeader.h"
#include <unordered_map>

namespace Anni
{
	namespace CI
	{
		[[nodiscard]] VkBufferViewCreateInfo PopulateUniformBufViewCI(const VkFormat buf_format);

	};        // namespace CI

	class BufViewWrapper
	{
	public:
		BufViewWrapper(DeviceManager& device_man_, VkBufferViewCreateInfo buf_view_CI_);

		~BufViewWrapper();
		VkBufferView GetRawBufView();

		BufViewWrapper() = delete;
		BufViewWrapper(const BufViewWrapper&) = delete;
		BufViewWrapper& operator=(const BufViewWrapper&) = delete;

		BufViewWrapper(BufViewWrapper&&) = delete;
		BufViewWrapper& operator=(BufViewWrapper&&) = delete;

	private:
		DeviceManager& device_manager;
		VkBufferViewCreateInfo buf_view_CI;
		VkBufferView           buffer_view;
	};

}        // namespace Anni
