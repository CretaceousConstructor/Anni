#pragma once
#include "AnniVkHeader.h"

namespace Anni
{

	struct BufferCreateInfoEnhanced
	{
		BufferCreateInfoEnhanced() = delete;

		constexpr BufferCreateInfoEnhanced(
			vk::BufferUsageFlags    usage_,
			vk::SharingMode         sharing_mode_,
			vk::MemoryPropertyFlags memory_properties_) :
			mem_prop(memory_properties_)
		{
			vk_buffer_CI.usage = usage_;
			vk_buffer_CI.sharingMode = sharing_mode_;
		}

		~BufferCreateInfoEnhanced() = default;
		BufferCreateInfoEnhanced(const BufferCreateInfoEnhanced&) = default;
		BufferCreateInfoEnhanced& operator=(const BufferCreateInfoEnhanced&) = default;

		BufferCreateInfoEnhanced(BufferCreateInfoEnhanced&&) = default;
		BufferCreateInfoEnhanced& operator=(BufferCreateInfoEnhanced&&) = default;

		vk::MemoryPropertyFlags mem_prop{};
		vk::MemoryRequirements mem_req{};
		vk::MemoryAllocateInfo  mem_alloc_info{};

		vk::BufferCreateInfo    vk_buffer_CI{};
	};

	namespace CI
	{
		bool IsSameType(const BufferCreateInfoEnhanced& lhs, const BufferCreateInfoEnhanced& rhs);

		constexpr BufferCreateInfoEnhanced StagingBuffer
		{
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};

		constexpr BufferCreateInfoEnhanced UniformBuffer
		{
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};

		constexpr BufferCreateInfoEnhanced VertexBuffer{
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eDeviceLocal };

		constexpr BufferCreateInfoEnhanced StorageBufferAddressable
		{
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		};

		constexpr BufferCreateInfoEnhanced IndexBuffer
		{
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		};





		//constexpr BufferCreateInfoEnhanced VertexBufferNonCoherent{VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT};
		//constexpr BufferCreateInfoEnhanced IndexBufferNonCoherent{VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT};

	}        // namespace CI

}        // namespace Anni
