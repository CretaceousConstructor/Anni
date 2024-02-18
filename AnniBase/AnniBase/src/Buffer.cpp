#include "Buffer.h"

namespace Anni
{
	Buffer::Buffer(
		QueueManager& queue_manager_,
		DeviceManager& device_manager_,
		VmaAllocatorWrapper& allocator_,
		VkTimelineSemPoolUnsafe& sem_pool_,

		const vk::Buffer buffer_,
		const VmaAllocation vma_allocation_,
		const BufferCreateInfoEnhanced& buf_CI_
	) :
		queue_manager(queue_manager_),
		device_manager(device_manager_),
		allocator(allocator_),
		sem_pool(sem_pool_),
		buf(buffer_),
		vma_allocation(vma_allocation_),
		buf_CI(buf_CI_)
	{
	}




	Buffer::~Buffer()
	{

		vmaDestroyBuffer(allocator.GetRaw(), buf, vma_allocation);

		//Unmap();
		//device_manager.GetLogicalDevice().destroyBuffer(buf);
		//device_manager.GetLogicalDevice().freeMemory(buf_mem);
	}



	//VkWriteDescriptorSet Buffer::GetWriteDescriptorSetInfo(uint32_t dstbinding, VkDescriptorType desc_type, uint32_t dstArrayElement, VkDeviceSize size, VkDeviceSize offset)
	//{
	//	des_buf_info.buffer = buf;
	//	des_buf_info.offset = offset;
	//	des_buf_info.range = size;

	//	VkWriteDescriptorSet temp_writeDescriptorSet{
	//		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//		.pNext = VK_NULL_HANDLE,
	//		.dstSet = VK_NULL_HANDLE,
	//		.dstBinding = dstbinding,
	//		.dstArrayElement = dstArrayElement,
	//		.descriptorCount = 1,
	//		.descriptorType = desc_type,
	//		.pBufferInfo = &des_buf_info,
	//	};
	//	return temp_writeDescriptorSet;
	//}

	//VkWriteDescriptorSet Buffer::GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, VkDescriptorType desc_type, uint32_t dstArrayElement, VkDeviceSize size, VkDeviceSize offset)
	//{
	//	des_buf_info.buffer = buf;
	//	des_buf_info.offset = offset;
	//	des_buf_info.range = size;

	//	VkWriteDescriptorSet temp_writeDescriptorSet{
	//		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
	//		.pNext = VK_NULL_HANDLE,
	//		.dstSet = set,
	//		.dstBinding = dstbinding,
	//		.dstArrayElement = dstArrayElement,
	//		.descriptorCount = 1,
	//		.descriptorType = desc_type,
	//		.pBufferInfo = &des_buf_info,
	//	};
	//	return temp_writeDescriptorSet;
	//}

	void Buffer::CopyFromHost(void const* outside_data_to_be_mapped, size_t outside_data_size, VkDeviceSize mapped_region_starting_offset)
	{

		//VmaAllocator allocator,
		//const void* pSrcHostPointer,
		//VmaAllocation dstAllocation,
		//VkDeviceSize dstAllocationLocalOffset,
		//VkDeviceSize size)
		//VK_CHECK_RESULT



		VK_CHECK_RESULT(
			vmaCopyMemoryToAllocation(
				allocator.GetRaw(),
				outside_data_to_be_mapped,
				vma_allocation,
				mapped_region_starting_offset,
				outside_data_size
			));

		//VULKAN_HPP_ASSERT(mapped_ptr_to_GPU_memory != nullptr, "This buffer is not host visible.");

		//memcpy(static_cast<void*>((static_cast<uint8_t*>(mapped_ptr_to_GPU_memory) + mapped_region_starting_offset)), outside_data_to_be_mapped, outside_data_size);

		//if (~(buf_CI.mem_prop & vk::MemoryPropertyFlagBits::eHostCoherent))
		//{
		//	Flush(outside_data_size, mapped_region_starting_offset);
		//}

		Buf2BufCopyInfo cpy_inf;
		cpy_inf.size = outside_data_size;
		cpy_inf.srcOffset = 0;
		cpy_inf.dstOffset = mapped_region_starting_offset;
		cpy_inf.trans_type = Buf2BufCopyInfo::TransferType::H2D;
		cpy_inf.queue = VK_NULL_HANDLE;
		cpy_inf.sem = VK_NULL_HANDLE;
		copy_infos.push_back(std::move(cpy_inf));
	}

	//void Buffer::MapMemory(VkDeviceSize size, VkDeviceSize offset)
	//{
	//	mapped_ptr_to_GPU_memory = device_manager.GetLogicalDevice().mapMemory(buf_mem, offset, size, vk::MemoryMapFlags(VK_ZERO_FLAG));
	//}

	//void Buffer::Flush(vk::DeviceSize size, vk::DeviceSize offset_within_whole_mem) const
	//{
	//	vk::MappedMemoryRange mappedRange = {};
	//	mappedRange.memory = buf_mem;
	//	mappedRange.offset = offset_within_whole_mem;
	//	mappedRange.size = size;

	//	device_manager.GetLogicalDevice().flushMappedMemoryRanges(mappedRange);
	//}

	//void Buffer::Unmap()
	//{
	//	if (mapped_ptr_to_GPU_memory)
	//	{
	//		device_manager.GetLogicalDevice().unmapMemory(buf_mem);
	//		mapped_ptr_to_GPU_memory = nullptr;
	//	}
	//}

	//void Buffer::Invalidate(vk::DeviceSize size, vk::DeviceSize offset_within_whole_mem) const
	//{
	//	vk::MappedMemoryRange mappedRange = {};
	//	mappedRange.memory = buf_mem;
	//	mappedRange.offset = offset_within_whole_mem;
	//	mappedRange.size = size;
	//	device_manager.GetLogicalDevice().invalidateMappedMemoryRanges(mappedRange);


	//}

	//void* Buffer::GetPtrToMappedRegion() const
	//{
	//	return mapped_ptr_to_GPU_memory;
	//}


	void Buffer::CopyFromStagingBuf(Buffer& providing_buf, Buf2BufCopyInfo copy_info)
	{
		//TODO: buffer range overlap testing检测overlap，然后根据overlap等待sem从而进行恰当的同步。
		Anni::Queue& queue_used_trans = queue_manager.GetMostDedicatedTransferQueue();
		Buffer& receiving_buf = *this;

		auto providing_buf_queue_fam = VK_QUEUE_FAMILY_IGNORED;
		auto receiving_buf_queue_fam = VK_QUEUE_FAMILY_IGNORED;

		Queue* last_providing_queue = nullptr;
		Queue* last_receiving_queue = nullptr;

		std::ranges::for_each(providing_buf.copy_infos | std::views::reverse, [&last_providing_queue](const auto& cpy_inf)
			{
				if (cpy_inf.queue != nullptr)
				{
					last_providing_queue = cpy_inf.queue;
				}
			});

		std::ranges::for_each(receiving_buf.copy_infos | std::views::reverse, [&last_receiving_queue](const auto& cpy_inf)
			{
				if (cpy_inf.queue != nullptr)
				{
					last_receiving_queue = cpy_inf.queue;
				}
			});


		if (last_receiving_queue)
		{
			receiving_buf_queue_fam = last_receiving_queue->GetQueueCap().queue_family_index;
		}

		if (last_providing_queue)
		{
			providing_buf_queue_fam = last_providing_queue->GetQueueCap().queue_family_index;
		}

		std::vector<vk::SemaphoreSubmitInfo> sem_wait_infos;
		//如果出现这种情况，就必须要等上一queue family的所有transfer，以及当前buffer的queue family的所有transfer操作完成。
		if (providing_buf_queue_fam != queue_used_trans.GetQueueCap().queue_family_index)
		{
			//必须等待source buffer使用完毕
			std::ranges::for_each(providing_buf.copy_infos,
				[&](const Buf2BufCopyInfo& cp_info)
				{
					if (cp_info.queue != &queue_used_trans && cp_info.queue == last_providing_queue)
					{
						constexpr auto waiting_stage = vk::PipelineStageFlagBits2::eCopy;
						sem_wait_infos.push_back(vk::SemaphoreSubmitInfo
						(
							cp_info.sem->GetRaw(),
							(cp_info.sem->GetLastValue() + 1),
							waiting_stage
						));
					}
				});
		};


		if (receiving_buf_queue_fam != queue_used_trans.GetQueueCap().queue_family_index)
		{
			//等待target buffer使用完毕
			std::ranges::for_each(receiving_buf.copy_infos,
				[&](const Buf2BufCopyInfo& cp_info)
				{
					if (cp_info.queue != &queue_used_trans && cp_info.queue == last_receiving_queue)
					{
						constexpr auto waiting_stage = vk::PipelineStageFlagBits2::eCopy;
						sem_wait_infos.push_back(vk::SemaphoreSubmitInfo
						(
							cp_info.sem->GetRaw(),
							(cp_info.sem->GetLastValue() + 1),
							waiting_stage
						));
					}
				});
		}

		auto sem_used_to_transfer = sem_pool.AcquireTimelineSem();

		Buf2BufCopyInfo src_cp_info;
		src_cp_info.size = copy_info.size;
		src_cp_info.srcOffset = copy_info.srcOffset;
		src_cp_info.dstOffset = copy_info.dstOffset;
		src_cp_info.trans_type = Buf2BufCopyInfo::TransferType::D2D_SRC;
		src_cp_info.queue = &queue_used_trans;
		src_cp_info.sem = sem_used_to_transfer;


		Buf2BufCopyInfo tar_cp_info;
		tar_cp_info.size = copy_info.size;
		tar_cp_info.srcOffset = copy_info.srcOffset;
		tar_cp_info.dstOffset = copy_info.dstOffset;
		tar_cp_info.trans_type = Buf2BufCopyInfo::TransferType::D2D_DST;
		tar_cp_info.queue = &queue_used_trans;
		tar_cp_info.sem = sem_used_to_transfer;

		const auto cmd_and_queue = queue_used_trans.BeginSingleTimeCommands();



		std::vector<vk::BufferMemoryBarrier2> accumulated_bars_4_providing_buf;
		std::ranges::for_each(providing_buf.copy_infos,
			[&](Buf2BufCopyInfo& history_cpy_inf)
			{
				vk::BufferMemoryBarrier2 temp_bar{};

				switch (history_cpy_inf.trans_type)
				{
				case Buf2BufCopyInfo::TransferType::H2D:


					temp_bar.srcAccessMask = vk::AccessFlagBits2::eHostWrite;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eHost;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = providing_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = providing_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;

				case Buf2BufCopyInfo::TransferType::D2H:
					temp_bar.srcAccessMask = vk::AccessFlagBits2::eHostRead;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eHost;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = providing_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = providing_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;

				case Buf2BufCopyInfo::TransferType::D2D_DST:
					temp_bar.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = providing_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = providing_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;

				case Buf2BufCopyInfo::TransferType::D2D_SRC:
					temp_bar.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = providing_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = providing_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;
				default:
					assert(!"Not a transfer type.");
				}
				accumulated_bars_4_providing_buf.push_back(temp_bar);
			}
		);

		std::vector<vk::BufferMemoryBarrier2> accumulated_bars_4_receiving_buf;
		std::ranges::for_each(receiving_buf.copy_infos,
			[&](Buf2BufCopyInfo& history_cpy_inf)
			{
				vk::BufferMemoryBarrier2 temp_bar{};

				switch (history_cpy_inf.trans_type)
				{
				case Buf2BufCopyInfo::TransferType::H2D:


					temp_bar.srcAccessMask = vk::AccessFlagBits2::eHostWrite;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eHost;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = receiving_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = receiving_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;

				case Buf2BufCopyInfo::TransferType::D2H:
					temp_bar.srcAccessMask = vk::AccessFlagBits2::eHostRead;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eHost;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = receiving_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = receiving_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;

				case Buf2BufCopyInfo::TransferType::D2D_DST:
					temp_bar.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = receiving_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = receiving_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;

				case Buf2BufCopyInfo::TransferType::D2D_SRC:
					temp_bar.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.srcStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
					temp_bar.dstStageMask = vk::PipelineStageFlagBits2::eCopy;

					temp_bar.srcQueueFamilyIndex = receiving_buf_queue_fam;
					temp_bar.dstQueueFamilyIndex = queue_used_trans.GetQueueCap().queue_family_index;

					temp_bar.buffer = receiving_buf.GetGPUBuffer();
					temp_bar.offset = src_cp_info.srcOffset;
					temp_bar.size = src_cp_info.size;
					break;
				default:
					assert(!"Not a transfer type.");
				}
				accumulated_bars_4_receiving_buf.push_back(temp_bar);
			}
		);

		providing_buf.copy_infos.push_back(src_cp_info);
		receiving_buf.copy_infos.push_back(tar_cp_info);


		vk::DependencyInfo depen_info_providing{};
		depen_info_providing.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info_providing.setBufferMemoryBarriers(accumulated_bars_4_providing_buf);

		vk::DependencyInfo depen_info_receiving{};
		depen_info_receiving.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info_receiving.setBufferMemoryBarriers(accumulated_bars_4_receiving_buf);

		cmd_and_queue.first.pipelineBarrier2(depen_info_providing);
		cmd_and_queue.first.pipelineBarrier2(depen_info_receiving);


		vk::BufferCopy2 copyRegion(
			copy_info.srcOffset,
			copy_info.dstOffset,
			copy_info.size
		);

		vk::CopyBufferInfo2 cpy_buf_info(
			providing_buf.buf,
			receiving_buf.buf,
			copyRegion
		);

		cmd_and_queue.first.copyBuffer2(cpy_buf_info);
		cmd_and_queue.first.end();

		vk::SemaphoreSubmitInfo sig_smt_inf{};
		sig_smt_inf.semaphore = sem_used_to_transfer->GetRaw();
		sig_smt_inf.value = (sem_used_to_transfer->GetLastValue() + 1);
		sig_smt_inf.stageMask = vk::PipelineStageFlagBits2::eCopy;

		vk::CommandBufferSubmitInfo buf_smt_inf(cmd_and_queue.first);

		vk::SubmitInfo2 smt_inf{};
		smt_inf.setCommandBufferInfos(buf_smt_inf);
		smt_inf.setWaitSemaphoreInfos(sem_wait_infos);
		smt_inf.setSignalSemaphoreInfos(sig_smt_inf);

		vk::Queue cur_exe_q = cmd_and_queue.second->GetQueue();
		cur_exe_q.submit2(smt_inf);

		//TODO command buffer recycle



	}

	VkDeviceSize Buffer::BufSize() const
	{
		return buf_CI.vk_buffer_CI.size;
	}

	VkDeviceSize Buffer::BufMemSize() const
	{
		return buf_CI.vma_allocation_info.size;
	}

	vk::Buffer Buffer::GetGPUBuffer() const
	{
		return buf;
	}
}        // namespace Anni
