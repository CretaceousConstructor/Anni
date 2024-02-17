#pragma once
#include "GraphicsComponent.h"
#include "Buffer.h"
#include "BufferCI.h"
#include "VirtualBuffer.h"
#include <list>

namespace Anni
{
	class BufferFactory
	{
	public:
		[[nodiscard]] std::shared_ptr<Buffer> ProduceBufferReFac(VkDeviceSize N, BufferCreateInfoEnhanced buf_CI, const void* const data = nullptr);
		[[nodiscard]] Buffer::BufferPtrBundle ProduceBufferPtrArrayReFac(VkDeviceSize N, uint32_t bundle_size, BufferCreateInfoEnhanced para_pack);

	public:

		void ActualizeVirtualResource(RenderGraphV1::VirtualBuffer& vbuf)
		{
			VULKAN_HPP_ASSERT(vbuf.descriptor.has_value(), "No discriptor of the given resource is provided.");

			const auto& dis = vbuf.descriptor.value();
			vbuf.p_rsrc = ProduceBufferReFac(dis.buf_CI, dis.init_cpu_data);
		}

	private:
		[[nodiscard]] std::shared_ptr<Buffer> ProduceBufferReFac(BufferCreateInfoEnhanced buf_CI, const void* const data = nullptr)
		{
			ProduceBufferReFac(buf_CI.vk_buffer_CI.size, buf_CI, data);
		}


	private:
		void                               BindBufferToMem(vk::Buffer buffer, vk::DeviceMemory memory, vk::DeviceSize offset = 0) const;
	private:
		std::list<std::shared_ptr<Buffer>> transient_buffers;

	private:
		DeviceManager& device_manager;
		VkTimelineSemPoolUnsafe& sem_pool;

	public:
		BufferFactory(DeviceManager& device_manager_, VkTimelineSemPoolUnsafe& sem_pool_);
		~BufferFactory() = default;

		BufferFactory() = delete;
		BufferFactory(const BufferFactory&) = delete;
		BufferFactory& operator=(const BufferFactory&) = delete;
		BufferFactory(BufferFactory&&) = delete;
		BufferFactory& operator=(BufferFactory&&) = delete;
	};

}        // namespace Anni
