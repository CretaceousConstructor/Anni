#pragma once
#include "AnniMath.h"
#include "AnniVkHeader.h"
#include "BufferCI.h"
#include "GraphicsComponent.h"
#include "TimelineSemPoolUnsafe.h"
#include "RsrcUsage.h"
#include "SyncInfo.h"
#include "VmaAllocator.h"

namespace Anni
{
	class Buffer
	{
		friend class BufferFactory;
	public:
		using BufferPtrBundle = std::vector<std::shared_ptr<Buffer>>;
		using BufferPtr = std::shared_ptr<Buffer>;

		struct Descriptor
		{
			void* init_cpu_data{ nullptr };
			BufferCreateInfoEnhanced                buf_CI;
			std::optional<vk::BufferViewCreateInfo> buf_view_CI;
		};

	public:

		Buffer(
			QueueManager& queue_manager_,
			DeviceManager& device_manager_,
			VmaAllocatorWrapper& allocator_,
			VkTimelineSemPoolUnsafe& sem_pool_,

			const vk::Buffer buffer_,
			const VmaAllocation vma_allocation_,
			const BufferCreateInfoEnhanced& buf_CI_
		);

		~Buffer();

		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer(Buffer&&) = delete;
		Buffer& operator=(Buffer&&) = delete;
	public:
		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(vk::DescriptorType desc_type, uint32_t dstbinding, uint32_t dstArrayElement = 0, vk::DeviceSize size = vk::WholeSize, vk::DeviceSize offset = 0);
		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(vk::DescriptorType desc_type, vk::DescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0, vk::DeviceSize size = vk::WholeSize, vk::DeviceSize offset = 0);
		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(RenderGraphV1::BufUsage& usage, vk::DescriptorSet set);
	public:
		[[nodiscard]] vk::Buffer GetGPUBuffer() const;
		void          CopyFromHost(void const* outside_data_to_be_mapped, size_t outside_data_size, vk::DeviceSize mapped_region_starting_offset = 0);
		void          CopyFromBuf(Buffer& providing_buf, const Buf2BufCopyInfo& copy_info);

		BufSyncInfo& GetSynInfoOnLoad();

		const std::vector<Buf2BufCopyInfo>& GetB2BCpyInfo()
		{
			return copy_infos;
		}


	public:
		[[nodiscard]] VkDeviceSize BufSize() const;
		[[nodiscard]] VkDeviceSize BufMemSize() const;


	private:
		QueueManager& queue_manager;
		DeviceManager& device_manager;
		VmaAllocatorWrapper& allocator;
		VkTimelineSemPoolUnsafe& sem_pool;
	private:
		vk::Buffer                      buf;
		VmaAllocation                   vma_allocation{};
	private:
		vk::DescriptorBufferInfo        des_buf_info;
	private:
		std::vector<Buf2BufCopyInfo>        copy_infos;
		BufSyncInfo                         sync_info_onload;
		std::shared_ptr<TimelineSemWrapper> sem_onload;
	private:
		BufferCreateInfoEnhanced       buf_CI;
	};

}        // namespace Anni
