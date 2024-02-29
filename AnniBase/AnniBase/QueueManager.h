#pragma once
#include "DeviceManager.h"
#include "AnniVkHeader.h"
#include "WindowsSys.h"
#include "TimelineSemWrapper.h"
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
		vk::CommandBuffer GetOneBuf();
		std::pair<vk::CommandBuffer, Queue* const> BeginSingleTimeCommands();
		void EndSingleTimeCopyCommands(std::pair<vk::CommandBuffer, Queue* const> cmd_buf_and_q, std::shared_ptr<TimelineSemWrapper> sem);

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