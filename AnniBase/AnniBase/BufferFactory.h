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
		[[nodiscard]] Buffer::BufferPtrBundle ProduceBufferPtrArray(VkDeviceSize N, uint32_t bundle_size, BufferCreateInfoEnhanced para_pack);
		[[nodiscard]] std::shared_ptr<Buffer> ProduceBuffer(VkDeviceSize N, BufferCreateInfoEnhanced buf_CI, const void* const data = nullptr);

	public:
		void ActualizeVirtualResource(RenderGraphV1::VirtualBuffer& vbuf)
		{
			VULKAN_HPP_ASSERT(vbuf.descriptor.has_value(), "No discriptor of the given resource is provided.");

			const auto& dis = vbuf.descriptor.value();
			vbuf.p_rsrc = ProduceBuffer(dis.buf_CI, dis.init_cpu_data);
		}

	private:
		[[nodiscard]] std::shared_ptr<Buffer> ProduceBuffer(BufferCreateInfoEnhanced buf_CI, const void* const data = nullptr)
		{
			ProduceBuffer(buf_CI.vk_buffer_CI.size, buf_CI, data);
		}


	private:
		std::list<std::shared_ptr<Buffer>> transient_buffers;

	private:
		QueueManager& queue_manager;
		DeviceManager& device_manager;
		VmaAllocatorWrapper& vam_allocator;
		VkTimelineSemPoolUnsafe& sem_pool;

	public:
		BufferFactory(
			QueueManager& queue_manager_,
			DeviceManager& device_manager_,
			VmaAllocatorWrapper& vam_allocator_,
			VkTimelineSemPoolUnsafe& sem_pool_
		);
		~BufferFactory() = default;

		BufferFactory() = delete;
		BufferFactory(const BufferFactory&) = delete;
		BufferFactory& operator=(const BufferFactory&) = delete;
		BufferFactory(BufferFactory&&) = delete;
		BufferFactory& operator=(BufferFactory&&) = delete;



	};

}        // namespace Anni
