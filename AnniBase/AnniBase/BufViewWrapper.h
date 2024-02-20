#pragma once

#include "DeviceManager.h"
#include "AnniVkHeader.h"
#include <unordered_map>

namespace Anni
{
	namespace CI
	{
		[[nodiscard]] vk::BufferViewCreateInfo PopulateUniformBufViewCI(const vk::Format buf_format);

	};        // namespace CI

	class BufViewWrapper
	{
	public:
		BufViewWrapper(DeviceManager& device_man_, vk::BufferViewCreateInfo buf_view_CI_);

		~BufViewWrapper() ;
		vk::BufferView& GetRawBufView();

		BufViewWrapper() = delete;
		BufViewWrapper(const BufViewWrapper&) = delete;
		BufViewWrapper& operator=(const BufViewWrapper&) = delete;

		BufViewWrapper(BufViewWrapper&&) = delete;
		BufViewWrapper& operator=(BufViewWrapper&&) = delete;

	private:
		DeviceManager& device_manager;
		vk::BufferViewCreateInfo buf_view_CI;
		vk::BufferView     buffer_view;
	};

}        // namespace Anni
