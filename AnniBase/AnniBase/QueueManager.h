#pragma once
#include "DeviceManager.h"
#include "AnniVkHeader.h"
#include "WindowsSys.h"
#include "PoolBundle.h"
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace Anni
{
	struct QueueCapability
	{
		bool   graphics{ false };
		bool   compute{ false };
		bool   transfer{ false };
		bool   present{ false };
		uint32_t queue_family_index{};
		uint32_t queue_index{};
	};

	class Queue
	{
	public:
		friend class QueueManager;
		Queue(vk::Queue queue_, QueueCapability queue_cap_, DeviceManager& device_man_);

		~Queue();

		Queue() = delete;
		Queue(const Queue&) = delete;
		Queue& operator=(const Queue&) = delete;

		Queue(Queue&& rhs) = delete;
		Queue& operator=(Queue&&) = delete;

		QueueCapability GetQueueCap() const;
		vk::Queue GetQueue() const;

		size_t GetNumDisbatchedPasses();
		void SetNumDisbatchedPasses(size_t n);

		std::pair<vk::CommandBuffer, vk::Semaphore> GetOneBufAndAssociatedSem();

		void FinalizeGeneralRendering(std::vector<vk::Semaphore> waiting_sems,const uint32_t cur_frame, vk::Semaphore frame_num_semaphore);

		vk::Semaphore GetTimeLineSem();

		vk::CommandBuffer GetOneBuf()
		{
			//get BUFFER
			vk::CommandBufferAllocateInfo cmd_buf_alloc_info{};

			cmd_buf_alloc_info.level = vk::CommandBufferLevel::ePrimary;
			cmd_buf_alloc_info.commandPool = cmd_pool.get();
			cmd_buf_alloc_info.commandBufferCount = 1;
			std::vector<vk::CommandBuffer> temp_buf = device_man.GetLogicalDevice().allocateCommandBuffers(cmd_buf_alloc_info);

			cmd_bufs.insert(temp_buf.front());
		}

		std::pair<vk::CommandBuffer, Queue* const> BeginSingleTimeCommands()
		{
			/*typedef struct VkCommandBufferAllocateInfo {
				VkStructureType         sType;
				const void* pNext;      gotta be nullptr
				VkCommandPool           commandPool;
				VkCommandBufferLevel    level;
				uint32_t                commandBufferCount;
			} VkCommandBufferAllocateInfo;*/


			vk::CommandBufferAllocateInfo cmd_buf_alloc_info{};

			cmd_buf_alloc_info.level = vk::CommandBufferLevel::ePrimary;
			cmd_buf_alloc_info.commandPool = cmd_pool.get();
			cmd_buf_alloc_info.commandBufferCount = 1;

			std::vector<vk::CommandBuffer> temp_buf = device_man.GetLogicalDevice().allocateCommandBuffers(cmd_buf_alloc_info);

			vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			temp_buf.front().begin(begin_info);

			return { temp_buf.front(), this };
		}



		void EndSingleTimeCopyCommands(std::pair<vk::CommandBuffer, Queue* const> cmd_buf_and_q, std::shared_ptr<TimelineSemWrapper> sem)
		{
			//VULKAN_HPP_ASSERT(cmd_buf_and_q.second == this);
			//cmd_buf_and_q.first.end();

			//vk::SemaphoreSubmitInfo sig_smt_inf{};
			//sig_smt_inf.semaphore = sem->GetRaw(),
			//sig_smt_inf.value = sem->GetLastValue() + 1;
			//sig_smt_inf.stageMask = vk::PipelineStageFlagBits2::eCopy;

			//vk::CommandBufferSubmitInfo buf_smt_inf(cmd_buf_and_q.first);

			//vk::SubmitInfo2 smt_inf{};
			//smt_inf.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			//smt_inf.pNext = VK_NULL_HANDLE;
			//smt_inf.flags = Vk::NoneFlag;
			//smt_inf.waitSemaphoreInfoCount = 0;
			//smt_inf.pWaitSemaphoreInfos = VK_NULL_HANDLE;



			//smt_inf.commandBufferInfoCount = 1;
			//smt_inf.pCommandBufferInfos = &buf_smt_inf;

			//smt_inf.signalSemaphoreInfoCount = 1;
			//smt_inf.pSignalSemaphoreInfos = &sig_smt_inf;

			//VK_CHECK_RESULT(vkQueueSubmit2(queue, 1, &smt_inf, nullptr));

			//TODO: command bufferªÿ ’
			//vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
		}


	public:
		std::unordered_map<vk::CommandBuffer, vk::Semaphore> buf_to_sem;

	private:
		size_t                              num_disbatched_passes;
		vk::Queue                           queue;
		QueueCapability                     queue_cap;

		vk::UniqueSemaphore                 timeline_semaphore;
		vk::UniqueCommandPool               cmd_pool;

		std::unordered_set<vk::CommandBuffer> cmd_bufs;
		DeviceManager& device_man;

	};






	class QueueManager
	{
	public:
		QueueManager(DeviceManager& device_man_, WindowsSys& window_);
		[[nodiscard]] Anni::Queue& GetSuitableQueue(QueueCapability required_queue_cap);
		void                       QueuesNumDisbatchedPassesToZero();
		[[nodiscard]] Anni::Queue& GetMostDedicatedTransferQueue();

	private:
		std::list<Anni::Queue> queues;
	private:
		DeviceManager& device_man;
		WindowsSys& window;

	public:
		QueueManager() = delete;
		~QueueManager() = default;

		QueueManager(const QueueManager&) = delete;
		QueueManager& operator=(const QueueManager&) = delete;

		QueueManager(QueueManager&&) = delete;
		Queue& operator=(Queue&&) = delete;
	};

}        // namespace Anni