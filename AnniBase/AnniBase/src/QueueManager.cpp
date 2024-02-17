#include "QueueManager.h"

namespace Anni
{

	Anni::Queue& QueueManager::GetSuitableQueue(QueueCapability required_queue_cap)
	{
		std::vector<Anni::Queue*> suitable_queue;

		for (auto& queue : queues)
		{
			auto cur_queue_cap = queue.GetQueueCap();
			if (required_queue_cap.graphics && !cur_queue_cap.graphics)
			{
				continue;
			}
			if (required_queue_cap.compute && !cur_queue_cap.compute)
			{
				continue;
			}
			if (required_queue_cap.transfer && !cur_queue_cap.transfer)
			{
				continue;
			}
			if (required_queue_cap.present && !cur_queue_cap.present)
			{
				continue;
			}
			suitable_queue.push_back(&queue);
		}


		std::ranges::sort(suitable_queue, [](const auto& rhs, const auto& lhs)
			{
				return rhs->GetNumDisbatchedPasses() < lhs->GetNumDisbatchedPasses();
			});

		Anni::Queue* result = suitable_queue.front();
		return *result;
	}


	void QueueManager::QueuesNumDisbatchedPassesToZero()
	{
		for (auto& queue : queues)
		{
			queue.SetNumDisbatchedPasses(0);
		}
	}



	Anni::Queue& QueueManager::GetMostDedicatedTransferQueue()
	{
		std::vector<std::pair<Anni::Queue*, size_t>> candidate;

		for (auto& queue : queues)
		{
			size_t      score = 0;
			const auto& queue_cap = queue.GetQueueCap();
			if (queue_cap.present)
				score++;
			if (queue_cap.compute)
				score++;
			if (queue_cap.graphics)
				score++;
			if (queue_cap.transfer)
				score++;

			if (queue_cap.transfer)
			{
				candidate.emplace_back(&queue_cap, score);
			}
		}
		std::ranges::sort(candidate,
			[](std::pair<Anni::Queue*, size_t>& lhs, std::pair<Anni::Queue*, size_t>& rhs)
			{
				return lhs.second < rhs.second;
			});

		Anni::Queue* p_queue = candidate.front().first;
		return *p_queue;
	}

	QueueManager::QueueManager(DeviceManager& device_man_, WindowsSys& window_) :
		device_man(device_man_), window(window_)
	{

		const auto& queue_families_props = device_man.GetQueueFamilyProps();
		for (size_t i = 0; i < queue_families_props.size(); ++i)
		{
			QueueCapability capability;
			//FILL UP QUEUE CAPABILITY
			capability.queue_family_index = i;
			capability.present = device_man.GetPhysicalDevice().getSurfaceSupportKHR(i, window.GetSurface());

			if (queue_families_props[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				capability.graphics = true;
			}
			if (queue_families_props[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute)
			{
				capability.compute = true;
			}
			if (queue_families_props[i].queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
			{
				capability.transfer = true;
			}

			//每种队列家族创建了尽可能多个queue
			for (int j = 0; j < queue_families_props[i].queueFamilyProperties.queueCount; ++j)
			{
				vk::Queue temp_queue;
				capability.queue_index = j;
				temp_queue = device_man.GetLogicalDevice().getQueue(i, j);
				queues.emplace_back(temp_queue, capability, device_man);
			}
		}
	}

	Queue::Queue(vk::Queue queue_, QueueCapability queue_cap_, DeviceManager& device_man_) :
		num_disbatched_passes(0u),
		queue(queue_),
		queue_cap(queue_cap_),
		device_man(device_man_)
	{
		const vk::Device logical_device = device_man.GetLogicalDevice();
		//*****************************************************************
		vk::SemaphoreTypeCreateInfo timelineCreateInfo(vk::SemaphoreType::eTimeline, 0, VK_NULL_HANDLE);
		vk::SemaphoreCreateInfo createInfo(vk::SemaphoreCreateFlags(VK_ZERO_FLAG), &timelineCreateInfo);
		timeline_semaphore = logical_device.createSemaphoreUnique(createInfo);

		//*****************************************************************
		vk::CommandPoolCreateInfo command_pool_CI(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			queue_cap.queue_family_index,
			VK_NULL_HANDLE
		);

		cmd_pool = logical_device.createCommandPoolUnique(command_pool_CI);
	}



	Queue::~Queue()
	{
		for (const auto& [_, sem] : buf_to_sem)
		{
			device_man.GetLogicalDevice().destroySemaphore(sem);
		}

	}


	QueueCapability Queue::GetQueueCap() const
	{
		return queue_cap;
	}

	vk::Queue Queue::GetQueue() const
	{
		return queue;
	}

	size_t Queue::GetNumDisbatchedPasses()
	{
		return num_disbatched_passes;
	}

	void Queue::SetNumDisbatchedPasses(size_t n)
	{
		num_disbatched_passes = n;
	}



	std::pair<vk::CommandBuffer, vk::Semaphore> Queue::GetOneBufAndAssociatedSem()
	{
		//get BUFFER
		vk::CommandBufferAllocateInfo buffer_alloc_info
		(
			cmd_pool.get(),
			vk::CommandBufferLevel::eSecondary,
			1
		);
		std::vector<vk::CommandBuffer> temp_bufs = device_man.GetLogicalDevice().allocateCommandBuffers(buffer_alloc_info);

		//get Timeline Semaphore
		//*****************************************************************
		vk::SemaphoreTypeCreateInfo timelineCreateInfo(vk::SemaphoreType::eTimeline, 0, VK_NULL_HANDLE);
		vk::SemaphoreCreateInfo createInfo(vk::SemaphoreCreateFlags(VK_ZERO_FLAG), &timelineCreateInfo);
		vk::Semaphore temp_sem = device_man.GetLogicalDevice().createSemaphore(createInfo);

		buf_to_sem.emplace(temp_bufs.front(), temp_sem);
		return { temp_bufs.front(), temp_sem };
	}




	void Queue::FinalizeGeneralRendering(std::vector<vk::Semaphore> waiting_sems, const uint32_t cur_frame, vk::Semaphore frame_num_semaphore)
	{
		assert(false, "not yet implemented");

		std::vector<vk::SemaphoreSubmitInfo> wait_sem_infos;

		std::ranges::transform
		(waiting_sems, std::back_inserter(wait_sem_infos),
			[](vk::Semaphore& sem)
			{
				return VkSemaphoreSubmitInfo{
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
					.pNext = VK_NULL_HANDLE,
					.semaphore = sem,
					.value = 1,
					.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,    //optimization later
					.deviceIndex = 0
				};
			}
		);

		std::vector<vk::SemaphoreSubmitInfo> signal_sem_infos;

		signal_sem_infos.push_back(
			vk::SemaphoreSubmitInfo(frame_num_semaphore, cur_frame + 1, vk::PipelineStageFlagBits2::eAllCommands)
		);



		vk::SubmitInfo2 submit_info{};
		submit_info.setWaitSemaphoreInfos(wait_sem_infos)
			.setSignalSemaphoreInfos(signal_sem_infos);

		queue.submit2(submit_info);
	};


	vk::Semaphore Queue::GetTimeLineSem()
	{
		return *timeline_semaphore;
	}


}        // namespace Anni
