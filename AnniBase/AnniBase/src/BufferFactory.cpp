#include "BufferFactory.h"
namespace Anni
{
	BufferFactory::BufferFactory(DeviceManager& device_manager_, VkTimelineSemPoolUnsafe& sem_pool_) :
		device_manager(device_manager_), sem_pool(sem_pool_)
	{
	}

	std::shared_ptr<Buffer> BufferFactory::ProduceBufferReFac(VkDeviceSize N, BufferCreateInfoEnhanced buf_CI, const void* const data)
	{
		assert(buf_CI.vk_buffer_CI.sharingMode != vk::SharingMode::eConcurrent);        //we deal with ownership transfer ourselves!!
		buf_CI.vk_buffer_CI.size = N;

		auto BuildBuffer = [&]()->vk::Buffer
			{
				vk::Buffer result = device_manager.GetLogicalDevice().createBuffer(buf_CI.vk_buffer_CI);
				return result;
			};

		const vk::Buffer buffer = BuildBuffer();

		auto BuildMem = [&]() -> vk::DeviceMemory
			{
				vk::DeviceMemory result{};
				buf_CI.mem_req = device_manager.GetLogicalDevice().getBufferMemoryRequirements2(buffer).memoryRequirements;

				buf_CI.mem_alloc_info.allocationSize = buf_CI.mem_req.size;
				buf_CI.mem_alloc_info.memoryTypeIndex = DeviceManager::FindMemoryType(buf_CI.mem_req.memoryTypeBits, buf_CI.mem_prop, device_manager.GetPhysicalDevice());

				vk::DeviceMemory result = device_manager.GetLogicalDevice().allocateMemory(buf_CI.mem_alloc_info);
				return result;
			};

		const vk::DeviceMemory buffer_memory = BuildMem();

		BindBufferToMem(buffer, buffer_memory);

		std::shared_ptr<Buffer> result = std::make_shared<Buffer>(device_manager, buffer, buffer_memory, buf_CI);



		//把staging buf管理起来，免得拷贝没有完成就销毁。（异步拷贝）
		if (CI::IsSameType(buf_CI, CI::StagingBuffer))
		{
			if (transient_buffers.size() > 100u) //也可以维护一个transient staging buffer total size in use，但是为了方便，现在这么写。回收频率比较慢，
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

		if (buf_CI.mem_prop & vk::MemoryPropertyFlagBits::eHostVisible)        //host visiable则可以用vkMapMemory函数
		{
			result->MapMemory(VK_WHOLE_SIZE, 0);
			if (data)
			{
				result->CopyFromHost(data, N);

				result->sync_info_onload =
					BufSyncInfo{
						.access_mask = vk::AccessFlagBits2::eHostWrite,
						.stage_mask = vk::PipelineStageFlagBits2::eHost
				};
				if (~(buf_CI.mem_prop & vk::MemoryPropertyFlagBits::eHostCoherent))
				{
					result->Flush();
				}
			}
			else
			{
				result->sync_info_onload =
					BufSyncInfo{
						.access_mask = vk::AccessFlagBits2::eNone,
						.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe
				};
			}
			//No need to unmap too early.
		}
		return result;
	}

	Buffer::BufferPtrBundle BufferFactory::ProduceBufferPtrArrayReFac(VkDeviceSize N, uint32_t bundle_size, BufferCreateInfoEnhanced buf_CI)
	{
		std::vector<std::shared_ptr<Buffer>> result_bundle;
		for (size_t i = 0; i < bundle_size; i++)
		{
			result_bundle.push_back(ProduceBufferReFac(N, buf_CI));
		}
		return result_bundle;
	}

	void BufferFactory::BindBufferToMem(vk::Buffer buffer, vk::DeviceMemory memory, vk::DeviceSize offset = 0) const
	{
		device_manager.GetLogicalDevice().bindBufferMemory(buffer, memory, offset);
	}



}        // namespace Anni
