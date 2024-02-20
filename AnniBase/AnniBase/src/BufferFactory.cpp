#include "BufferFactory.h"
namespace Anni
{
	BufferFactory::BufferFactory(
		QueueManager& queue_manager_,
		DeviceManager& device_manager_,
		VmaAllocatorWrapper& vam_allocator_,
		VkTimelineSemPoolUnsafe& sem_pool_
	) :
		queue_manager(queue_manager_), device_manager(device_manager_), vam_allocator(vam_allocator_), sem_pool(sem_pool_)
	{
	}




	std::shared_ptr<Buffer> BufferFactory::ProduceBuffer(VkDeviceSize N, BufferCreateInfoEnhanced buf_CI, const void* const data)
	{

		buf_CI.vk_buffer_CI.size = N;
		//let the VMA library know that this data should be writeable by CPU, but also readable by GPU

		VkBuffer      temp_buffer{};
		VmaAllocation vma_allocation{};
		VkBufferCreateInfo vk_buffer_CI = buf_CI.vk_buffer_CI;

		VK_CHECK_RESULT(
			vmaCreateBuffer(
				vam_allocator.GetRaw(),
				&vk_buffer_CI,
				&buf_CI.vma_allocation_CI,
				&temp_buffer,
				&vma_allocation,
				&buf_CI.vma_allocation_info
			));

		vk::Buffer result_buffer = temp_buffer;

		std::shared_ptr<Buffer> result = std::make_shared<Buffer>(queue_manager, device_manager, vam_allocator, sem_pool, result_buffer, vma_allocation, buf_CI);

		//��staging buf������������ÿ���û����ɾ����١����첽������
		if (CI::IsSameType(buf_CI, CI::StagingBuffer))
		{
			if (transient_buffers.size() > 100u) //Ҳ����ά��һ��transient staging buffer total size in use������Ϊ�˷��㣬������ôд������Ƶ�ʱȽ�����
			{
				auto& candidate_recycle_buf = transient_buffers.front();
				bool all_signaled = std::ranges::all_of
				(candidate_recycle_buf->copy_infos,
					[](const Buf2BufCopyInfo& buf_cp_inf)
					{
						return ((buf_cp_inf.sem->GetLastValue() + 1) == buf_cp_inf.sem->ReturnState());
					}
				);

				if (all_signaled)
				{
					transient_buffers.pop_front();
				}
			}
			transient_buffers.push_back(result);
		}

		if (data)
		{
			result->CopyFromHost(data, N);
			result->sync_info_onload =
				BufSyncInfo{
					.access_mask = vk::AccessFlagBits2::eHostWrite,
					.stage_mask = vk::PipelineStageFlagBits2::eHost
			};
		}
		else
		{
			result->sync_info_onload =
				BufSyncInfo{
					.access_mask = vk::AccessFlagBits2::eNone,
					.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe
			};
		}
		return result;
	}

	void BufferFactory::ActualizeVirtualResource(RenderGraphV1::VirtualBuffer& vbuf)
	{
		ASSERT_WITH_MSG(vbuf.descriptor.has_value(), "No discriptor of the given resource is provided.");

		const auto& dis = vbuf.descriptor.value();
		vbuf.p_rsrc = ProduceBuffer(dis.buf_CI, dis.init_cpu_data);
	}

	std::shared_ptr<Buffer> BufferFactory::ProduceBuffer(BufferCreateInfoEnhanced buf_CI, const void* const data)
	{
		return ProduceBuffer(buf_CI.vk_buffer_CI.size, buf_CI, data);
	}






	//std::shared_ptr<Buffer> BufferFactory::ProduceBufferReFac(VkDeviceSize N, BufferCreateInfoEnhanced buf_CI, const void* const data)
	//{
	//	VULKAN_HPP_ASSERT(buf_CI.vk_buffer_CI.sharingMode != vk::SharingMode::eConcurrent);        //we deal with ownership transfer ourselves!!
	//	buf_CI.vk_buffer_CI.size = N;

	//	auto BuildBuffer = [&]()->vk::Buffer
	//		{
	//			vk::Buffer result = device_manager.GetLogicalDevice().createBuffer(buf_CI.vk_buffer_CI);
	//			return result;
	//		};

	//	const vk::Buffer buffer = BuildBuffer();

	//	auto BuildMem = [&]() -> vk::DeviceMemory
	//		{
	//			vk::DeviceMemory result{};
	//			buf_CI.mem_req = device_manager.GetLogicalDevice().getBufferMemoryRequirements2(buffer).memoryRequirements;

	//			buf_CI.mem_alloc_info.allocationSize = buf_CI.mem_req.size;
	//			buf_CI.mem_alloc_info.memoryTypeIndex = DeviceManager::FindMemoryType(buf_CI.mem_req.memoryTypeBits, buf_CI.mem_prop, device_manager.GetPhysicalDevice());

	//			vk::DeviceMemory result = device_manager.GetLogicalDevice().allocateMemory(buf_CI.mem_alloc_info);
	//			return result;
	//		};

	//	const vk::DeviceMemory buffer_memory = BuildMem();

	//	BindBufferToMem(buffer, buffer_memory);

	//	std::shared_ptr<Buffer> result = std::make_shared<Buffer>(device_manager, buffer, buffer_memory, buf_CI);



	//	//��staging buf������������ÿ���û����ɾ����١����첽������
	//	if (CI::IsSameType(buf_CI, CI::StagingBuffer))
	//	{
	//		if (transient_buffers.size() > 100u) //Ҳ����ά��һ��transient staging buffer total size in use������Ϊ�˷��㣬������ôд������Ƶ�ʱȽ�����
	//		{
	//			auto& candidate_recycle_buf = transient_buffers.front();
	//			bool all_signaled = std::ranges::all_of
	//			(candidate_recycle_buf->copy_infos,
	//				[](const Buf2BufCopyInfo& buf_cp_inf)
	//				{
	//					return ((buf_cp_inf.sem->GetLastValue() + 1) == buf_cp_inf.sem->ReturnState());
	//				}
	//			);

	//			if (all_signaled)
	//			{
	//				transient_buffers.pop_front();
	//			}
	//		}
	//		transient_buffers.push_back(result);
	//	}

	//	if (buf_CI.mem_prop & vk::MemoryPropertyFlagBits::eHostVisible)        //host visiable�������vkMapMemory����
	//	{
	//		result->MapMemory(VK_WHOLE_SIZE, 0);
	//		if (data)
	//		{
	//			result->CopyFromHost(data, N);

	//			result->sync_info_onload =
	//				BufSyncInfo{
	//					.access_mask = vk::AccessFlagBits2::eHostWrite,
	//					.stage_mask = vk::PipelineStageFlagBits2::eHost
	//			};
	//			if (~(buf_CI.mem_prop & vk::MemoryPropertyFlagBits::eHostCoherent))
	//			{
	//				result->Flush();
	//			}
	//		}
	//		else
	//		{
	//			result->sync_info_onload =
	//				BufSyncInfo{
	//					.access_mask = vk::AccessFlagBits2::eNone,
	//					.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe
	//			};
	//		}
	//		//No need to unmap too early.
	//	}
	//	return result;
	//}

	Buffer::BufferPtrBundle BufferFactory::ProduceBufferPtrArray(VkDeviceSize N, uint32_t bundle_size, BufferCreateInfoEnhanced buf_CI)
	{
		std::vector<std::shared_ptr<Buffer>> result_bundle;
		for (size_t i = 0; i < bundle_size; i++)
		{
			result_bundle.push_back(ProduceBuffer(N, buf_CI));
		}
		return result_bundle;
	}




}        // namespace Anni
