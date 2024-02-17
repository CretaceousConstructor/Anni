#pragma once
#include "AnniMath.h"
#include "AnniVkHeader.h"
#include "BufferCI.h"
#include "GraphicsComponent.h"
#include "TimelineSemPoolUnsafe.h"
#include "RsrcUsage.h"
#include "SyncInfo.h"

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
			void* init_cpu_data;
			BufferCreateInfoEnhanced              buf_CI;
			std::optional<vk::BufferViewCreateInfo> buf_view_CI;
		};

	public:
		~Buffer();

		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer(Buffer&&) = delete;
		Buffer& operator=(Buffer&&) = delete;


	public:
		//OBSOLETE 
		//[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, VkDescriptorType desc_type, uint32_t dstArrayElement = 0, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		//[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, VkDescriptorType desc_type, uint32_t dstArrayElement = 0, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);


		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(RenderGraphV1::BufUsage& usage, vk::DescriptorSet set)
		{
			Vk::BufferSubRange buf_subrange = usage.GetSynInfo().buf_subrange.value_or(
				Vk::BufferSubRange{
					.offset = 0 ,
					.size = VK_WHOLE_SIZE
				});

			usage.des_buf_info.buffer = buf;
			usage.des_buf_info.offset = buf_subrange.offset;
			usage.des_buf_info.range = buf_subrange.size;


			vk::WriteDescriptorSet temp_writeDescriptorSet(
				set,
				usage.desc_info.slot_info.binding,
				usage.desc_info.slot_info.array_element,
				Constants::DescriptorCount<1>,
				usage.desc_info.descriptor_type
			);
			temp_writeDescriptorSet.setBufferInfo(usage.des_buf_info);

			return temp_writeDescriptorSet;
		}




		[[nodiscard]] vk::Buffer GetGPUBuffer() const;
		void          Invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset_within_whole_mem = 0) const;
		void          Flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset_within_whole_mem = 0) const;
		void          CopyFromHost(void const* outside_data_to_be_mapped, size_t outside_data_size, VkDeviceSize mapped_region_starting_offset = 0);
		void          CopyFromStagingBuf(Buffer& providing_buf, Buf2BufCopyInfo copy_info);


		BufSyncInfo GetSynInfoOnLoad()
		{
			return sync_info_onload;
		}


	public:
		[[nodiscard]] VkDeviceSize BufSize() const;
		[[nodiscard]] VkDeviceSize BufMemSize() const;

	private:
		Buffer(GraphicsComponent& gfx_, VkTimelineSemPoolUnsafe& sem_pool, const VkBuffer buffer_, const VkDeviceMemory buffer_memory_, BufferCreateInfoEnhanced buf_CI);
		[[nodiscard]] void* GetPtrToMappedRegion() const;
		void MapMemory(VkDeviceSize size, VkDeviceSize offset);
		void Unmap();

	private:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;
		VkTimelineSemPoolUnsafe& sem_pool;

	private:
		vk::Buffer                      buf;
		vk::DeviceMemory                buf_mem;
		void* mapped_ptr_to_GPU_memory{ nullptr };

		std::vector<Buf2BufCopyInfo>    copy_infos;
		vk::DescriptorBufferInfo        des_buf_info;

		BufSyncInfo                         sync_info_onload;
		std::shared_ptr<TimelineSemWrapper> sem_onload;

		BufferCreateInfoEnhanced       buf_CI;

	};

}        // namespace Anni
