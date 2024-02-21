#include "IPassNode.h"

namespace Anni::RenderGraphV1
{
	GraphicsPassNode::GraphicsPassNode(std::string name_, DeviceManager& device_manager_,
		SwapchainManager& swapchain_manager_,
		DescriptorLayoutManager& descriptor_set_layout_manager_,
		VkShaderFactory& shader_fac_,
		DescriptorSetAllocatorGrowable& descriptor_allocator_,
		std::list<VirtualBuffer>& rg_buffers_,
		std::list<VirtualTexture>& rg_textures_
	) :
		name(name_),
		device_manager(device_manager_),
		swapchain_manager(swapchain_manager_),

		descriptor_set_layout_manager(descriptor_set_layout_manager_),
		shader_fac(shader_fac_),
		descriptor_allocator(descriptor_allocator_),
		rg_buffers(rg_buffers_),
		rg_textures(rg_textures)
	{
	}


	void GraphicsPassNode::InsertSyncInfoForInitalUsage(
		const Anni::BufSyncInfo& source_syn_info,
		const Anni::BufSyncInfo& target_syn_info,
		VBufItr underlying_rsrc)
	{
		buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}

	void GraphicsPassNode::InsertSyncInfoForInitalUsage(
		const Anni::ImgSyncInfo& source_syn_info,
		const Anni::ImgSyncInfo& target_syn_info,
		VTexItr underlying_rsrc)
	{
		tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}


	void GraphicsPassNode::InsertSameQueueSyncInfo(
		GraphicsPassNode* const source_pass,
		GraphicsPassNode* const target_pass,
		Queue* const queue_,
		const Anni::BufSyncInfo& source_syn_info,
		const Anni::BufSyncInfo& target_syn_info,
		VBufItr underlying_rsrc)
	{
		ASSERT_WITH_MSG(this == target_pass, "this sync info shouldn't be interted in this pass.");
		buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}

	void GraphicsPassNode::InsertSameQueueSyncInfo(
		GraphicsPassNode* const source_pass,
		GraphicsPassNode* const target_pass,
		Queue* const queue_,
		const Anni::ImgSyncInfo& source_syn_info,
		const Anni::ImgSyncInfo& target_syn_info,
		VTexItr underlying_rsrc)
	{
		ASSERT_WITH_MSG(this == target_pass, "this sync info shouldn't be interted in this pass.");
		tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}


	void GraphicsPassNode::InsertDiffQueueSyncInfoTimeline(
		GraphicsPassNode* const source_pass,
		GraphicsPassNode* const target_pass,
		std::shared_ptr<TimelineSemWrapper>& sema_sync,
		Queue* const source_queue_,
		Queue* const target_queue_,
		const Anni::BufSyncInfo& source_syn_info,
		const Anni::BufSyncInfo& target_syn_info,
		VBufItr underlying_rsrc,

		SemInsertInfoSafe sor_sem_insersion_info,
		SemInsertInfoSafe tar_sem_insersion_info)
	{
		if (this == source_pass)
		{
			buf_syn_infos_tail_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_,
				underlying_rsrc);

			const auto key = PassNodePair{ source_pass, target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(
					key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), sor_sem_insersion_info));
			}
			else
			{
				pass_pair_to_t_sem.at(key).second.signal_stages |= sor_sem_insersion_info.signal_stages;
			}
		}
		else if (this == target_pass)
		{
			buf_syn_infos_head_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_,
				underlying_rsrc);

			const auto key = PassNodePair{ source_pass, target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(
					key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), tar_sem_insersion_info));
			}
			else
			{
				pass_pair_to_t_sem.at(key).second.wait_stages |= tar_sem_insersion_info.wait_stages;
			}
		}
		else
		{
			ASSERT_WITH_MSG(false, "this sync info shouldn't be interted here.");
		}
	}

	void GraphicsPassNode::InsertDiffQueueSyncInfoTimeline(
		GraphicsPassNode* const source_pass,
		GraphicsPassNode* const target_pass,
		std::shared_ptr<TimelineSemWrapper>& sema_sync,
		Queue* const source_queue_,
		Queue* const target_queue_,
		const Anni::ImgSyncInfo& source_syn_info,
		const Anni::ImgSyncInfo& target_syn_info,
		VTexItr underlying_rsrc,

		SemInsertInfoSafe sor_sem_insersion_info,
		SemInsertInfoSafe tar_sem_insersion_info)
	{
		if (this == source_pass)
		{
			tex_syn_infos_tail_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_,
				underlying_rsrc);

			const auto key = PassNodePair{ source_pass, target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(
					key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), sor_sem_insersion_info));
			}
			else
			{
				pass_pair_to_t_sem.at(key).second.signal_stages |= sor_sem_insersion_info.signal_stages;
			}
		}
		else if (this == target_pass)
		{
			tex_syn_infos_head_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_,
				underlying_rsrc);

			const auto key = PassNodePair{ source_pass, target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(
					key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), tar_sem_insersion_info));
			}
			else
			{
				pass_pair_to_t_sem.at(key).second.wait_stages |= tar_sem_insersion_info.wait_stages;
			}
		}
		else
		{
			ASSERT_WITH_MSG(false, "this sync info shouldn't be interted here.");
		}
	}


	void GraphicsPassNode::ResourcesAcquisition(VkTextureFactory& tex_fac, BufferFactory& buf_fac)
	{
		//REQUIRED RESOURCES INITIALIZATION
		for (auto& buf_itr : buf_init_list)
		{
			//VirtualResource has a descriptor to help initalize resources
			buf_fac.ActualizeVirtualResource(GetVRsrcFromRsrcItr(buf_itr));
		}

		for (auto& tex_itr : tex_init_list)
		{
			//VirtualResource has a descriptor to help initalize resources
			tex_fac.ActualizeVirtualResource(GetVRsrcFromRsrcItr(tex_itr));
		}
	}

	void GraphicsPassNode::SyncInsertAfter(vk::CommandBuffer cmd_buf,std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
	{
		SyncInsertAfterSameQ(cmd_buf, signal_sem_submit_info);
		SyncInsertAfterDiffQ(cmd_buf, signal_sem_submit_info);
	}

	void GraphicsPassNode::GenerateAllAccessStages()
	{
		std::ranges::for_each
		(
			buf_usages,
			[&](VirtualBufRsrcAndUsage& rsrc_usage)
			{
				all_access_stages |= rsrc_usage.usage.sync_info.stage_mask;
			}
		);

		std::ranges::for_each
		(
			tex_usages,
			[&](VirtualTexRsrcAndUsage& rsrc_usage)
			{
				all_access_stages |=
					std::visit
					([&](auto& variant_usage) -> vk::PipelineStageFlags2
						{
							return variant_usage.sync_info.stage_mask;
						}, rsrc_usage.usage
					);
			}
		);
	}


	void GraphicsPassNode::SyncInsertAheadSameQ(vk::CommandBuffer cmd_buf,
		std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info)
	{
		//wait for semaphores(even in the same queue，some resources are being loaded, thus needs to wait for semaphores' signal first).

		//std::vector<VkSemaphoreSubmitInfo> wait_sem_submit_info_on_same_q;
		//for (auto& head_syn : buf_syn_infos_head_same_q)
		//{
		//	for (const auto& sync_sem : head_syn.wait_sync_semas)
		//	{
		//		constexpr uint64_t wait_value = 1u;
		//		wait_sem_submit_info.push_back(
		//			VkSemaphoreSubmitInfo{
		//				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//				.pNext = VK_NULL_HANDLE,
		//				.semaphore = sync_sem->GetRaw(),
		//				.value = wait_value,
		//				.stageMask = head_syn.target_sync_info.stage_mask,
		//				.deviceIndex = 0
		//			}
		//		);
		//	}

		//	for (const auto& sync_sem : head_syn.sync_semas_binary)
		//	{
		//		constexpr uint64_t wait_value = 1u;
		//		wait_sem_submit_info.push_back(
		//			VkSemaphoreSubmitInfo{
		//				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//				.pNext = VK_NULL_HANDLE,
		//				.semaphore = sync_sem->GetRaw(),
		//				.value = wait_value,
		//				.stageMask = head_syn.target_sync_info.stage_mask,
		//				.deviceIndex = 0
		//			}
		//		);
		//	}
		//}

		//for (auto& head_syn : tex_syn_infos_head_same_q)
		//{
		//	for (const auto& sync_sem : head_syn.sync_semas)
		//	{
		//		constexpr uint64_t wait_value = 1u;
		//		wait_sem_submit_info.push_back(
		//			VkSemaphoreSubmitInfo{
		//				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//				.pNext = VK_NULL_HANDLE,
		//				.semaphore = sync_sem->GetRaw(),
		//				.value = wait_value,
		//				.stageMask = head_syn.target_sync_info.stage_mask,
		//				.deviceIndex = 0
		//			}
		//		);
		//	}


		//	for (const auto& sync_sem : head_syn.sync_semas_binary)
		//	{
		//		constexpr uint64_t wait_value = 1u;
		//		wait_sem_submit_info.push_back(
		//			VkSemaphoreSubmitInfo{
		//				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//				.pNext = VK_NULL_HANDLE,
		//				.semaphore = sync_sem->GetRaw(),
		//				.value = wait_value,
		//				.stageMask = head_syn.target_sync_info.stage_mask,
		//				.deviceIndex = 0
		//			}
		//		);
		//	}

		//}


		////no command buffers are needed in this case. We just wait for semaphores
		//auto submit_info = VkSubmitInfo2{
		//	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		//	.pNext = VK_NULL_HANDLE,
		//	.flags = Vk::NoneFlag,
		//	.waitSemaphoreInfoCount = static_cast<uint32_t>(wait_sem_submit_info_on_same_q.size()),
		//	.pWaitSemaphoreInfos = wait_sem_submit_info_on_same_q.data(),
		//	.commandBufferInfoCount = 0u,
		//	.pCommandBufferInfos = VK_NULL_HANDLE,
		//	.signalSemaphoreInfoCount = 0u,
		//	.pSignalSemaphoreInfos = VK_NULL_HANDLE
		//};

		//vkQueueSubmit2(execution_queue.GetQueue(), 1, &submit_info, VK_NULL_HANDLE);


		//After waiting, we can use barriers.
		std::vector<vk::BufferMemoryBarrier2> buf_bars;
		for (auto& head_syn : buf_syn_infos_head_same_q)
		{
			VirtualBuffer& buff = *head_syn.underlying_vrsrc;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info,
				head_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};


		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& head_syn : tex_syn_infos_head_same_q)
		{
			VirtualTexture& tex = *head_syn.underlying_vrsrc;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info,
				head_syn.target_sync_info);
			tex_bars.push_back(tex_barrier);
		}

		auto depen_info = vk::DependencyInfo{};
		depen_info.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info.setImageMemoryBarriers(tex_bars);
		depen_info.setBufferMemoryBarriers(buf_bars);
		cmd_buf.pipelineBarrier2(depen_info);
	}

	//std::vector<VkSemaphoreSubmitInfo> signal_sem_submit_info_on_diff_q;
	//for (auto& diffq_head_sync : buf_syn_infos_tail_diff_q)
	//{
	//	uint64_t signal_value = diffq_head_sync.signal_value.value_or(1);

	//	signal_sem_submit_info.push_back(
	//		VkSemaphoreSubmitInfo{
	//			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
	//			.pNext = VK_NULL_HANDLE,
	//			.semaphore = diffq_head_sync.sync_sema,
	//			.value = signal_value,
	//			.stageMask = diffq_head_sync.source_sync_info.stage_mask,
	//			.deviceIndex = 0 });
	//}

	//for (auto& diffq_head_sync : p_pass->uni_tex_syn_infos_tail_diff_q)
	//{
	//	uint64_t signal_value = diffq_head_sync.signal_value.value_or(1);

	//	signal_sem_submit_info.push_back(
	//		VkSemaphoreSubmitInfo{
	//			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
	//			.pNext = VK_NULL_HANDLE,
	//			.semaphore = diffq_head_sync.sync_sema,
	//			.value = signal_value,
	//			.stageMask = diffq_head_sync.source_sync_info.stage_mask,
	//			.deviceIndex = 0 });
	//}


	void GraphicsPassNode::SyncInsertAheadDiffQ(vk::CommandBuffer cmd_buf,
		std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info)
	{
		//for (auto& diffq_head_sync : buf_syn_infos_head_diff_q)
		//{
		//	uint64_t wait_value = diffq_head_sync.wait_value.value_or(1);
		//	wait_sem_submit_info.push_back(
		//		VkSemaphoreSubmitInfo{
		//			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//			.pNext = VK_NULL_HANDLE,
		//			.semaphore = diffq_head_sync.sync_sema->GetRaw(),
		//			.value = wait_value,
		//			.stageMask = diffq_head_sync.target_sync_info.stage_mask,
		//			.deviceIndex = 0 }
		//	);
		//}

		//for (auto& diffq_head_sync : tex_syn_infos_head_diff_q)
		//{
		//	uint64_t wait_value = diffq_head_sync.wait_value.value_or(1);
		//	wait_sem_submit_info.push_back(
		//		VkSemaphoreSubmitInfo{
		//			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//			.pNext = VK_NULL_HANDLE,
		//			.semaphore = diffq_head_sync.sync_sema->GetRaw(),
		//			.value = wait_value,
		//			.stageMask = diffq_head_sync.target_sync_info.stage_mask,
		//			.deviceIndex = 0 });
		//}

		//VkCommandBufferSubmitInfo cmd_submit_info{
		//	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		//	.pNext = VK_NULL_HANDLE,
		//	.commandBuffer = VK_NULL_HANDLE
		//};

		////no command buffers are needed in this case. We just wait for semaphores
		//auto submit_info = VkSubmitInfo2{
		//	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		//	.pNext = VK_NULL_HANDLE,
		//	.flags = Vk::NoneFlag,
		//	.waitSemaphoreInfoCount = static_cast<uint32_t>(wait_sem_submit_info_on_diff_q.size()),
		//	.pWaitSemaphoreInfos = wait_sem_submit_info_on_diff_q.data(),
		//	.commandBufferInfoCount = 0u,
		//	.pCommandBufferInfos = VK_NULL_HANDLE,
		//	.signalSemaphoreInfoCount = 0u,
		//	.pSignalSemaphoreInfos = VK_NULL_HANDLE
		//};

		//vkQueueSubmit2(execution_queue.GetQueue(), 1, &submit_info, VK_NULL_HANDLE);


		//After waiting, we can use barriers.
		std::vector<vk::BufferMemoryBarrier2> buf_bars;
		for (auto& head_syn : buf_syn_infos_head_diff_q)
		{
			VirtualBuffer& buff = *head_syn.underlying_vrsrc;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info,
				head_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};


		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& head_syn : tex_syn_infos_head_diff_q)
		{
			VirtualTexture& tex = *head_syn.underlying_vrsrc;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info,
				head_syn.target_sync_info);
			tex_bars.push_back(tex_barrier);
		}

		auto depen_info = vk::DependencyInfo{};
		depen_info.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info.setImageMemoryBarriers(tex_bars);
		depen_info.setBufferMemoryBarriers(buf_bars);
		cmd_buf.pipelineBarrier2(depen_info);


		//for (auto& head_syn : buf_syn_infos_head_diff_q)
		//{
		//	VirtualBuffer& buff = head_syn.underlying_vrsrc->second;
		//	VkBufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);

		//	buf_barrier.srcQueueFamilyIndex = head_syn.source_queue->GetQueueCap().queue_family_index;
		//	buf_barrier.dstQueueFamilyIndex = head_syn.target_queue->GetQueueCap().queue_family_index;

		//	VkDependencyInfo depen_info = VkDependencyInfo{
		//		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		//		.pNext = VK_NULL_HANDLE,
		//		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
		//		.memoryBarrierCount = 0,
		//		.pMemoryBarriers = VK_NULL_HANDLE,
		//		.bufferMemoryBarrierCount = 1,
		//		.pBufferMemoryBarriers = &buf_barrier,
		//		.imageMemoryBarrierCount = 0,
		//		.pImageMemoryBarriers = VK_NULL_HANDLE
		//	};

		//	vkCmdPipelineBarrier2(cmd_buf, &depen_info);
		//}


		//for (auto& head_syn : tex_syn_infos_head_diff_q)
		//{
		//	//std::vector<VkDependencyInfo> depens;
		//	VirtualTexture& tex = head_syn.underlying_vrsrc->second;
		//	auto            tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);

		//	tex_barrier.srcQueueFamilyIndex = head_syn.source_queue->GetQueueCap().queue_family_index;
		//	tex_barrier.dstQueueFamilyIndex = head_syn.target_queue->GetQueueCap().queue_family_index;

		//	auto depen_info = VkDependencyInfo{
		//		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		//		.pNext = VK_NULL_HANDLE,
		//		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
		//		.memoryBarrierCount = 0,
		//		.pMemoryBarriers = VK_NULL_HANDLE,
		//		.bufferMemoryBarrierCount = 0,
		//		.pBufferMemoryBarriers = VK_NULL_HANDLE,
		//		.imageMemoryBarrierCount = 1,
		//		.pImageMemoryBarriers = &tex_barrier };

		//	vkCmdPipelineBarrier2(cmd_buf, &depen_info);
		//}

		//auto submit_info = VkSubmitInfo2{
		//	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		//	.pNext = VK_NULL_HANDLE,
		//	.flags = Vk::NoneFlag,
		//	.waitSemaphoreInfoCount = static_cast<uint32_t>(wait_sem_submit_info_on_diff_q.size()),
		//	.pWaitSemaphoreInfos = wait_sem_submit_info_on_diff_q.data(),
		//	.commandBufferInfoCount = 0u,
		//	.pCommandBufferInfos = VK_NULL_HANDLE,
		//	.signalSemaphoreInfoCount = 0u,
		//	.pSignalSemaphoreInfos = VK_NULL_HANDLE
		//};

		//vkQueueSubmit2(execution_queue.GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
	}

	void GraphicsPassNode::SyncInsertAfterSameQ(vk::CommandBuffer cmd_buf,
		std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
	{
		//we can use barriers.
		//OPTIMIZATION: 合批

		//After waiting, we can use barriers.
		std::vector<vk::BufferMemoryBarrier2> buf_bars;
		for (auto& tail_syn : buf_syn_infos_tail_same_q)
		{
			VirtualBuffer& buff = *tail_syn.underlying_vrsrc;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(tail_syn.source_sync_info,
				tail_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};

		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& tail_syn : tex_syn_infos_tail_same_q)
		{
			VirtualTexture& tex = *tail_syn.underlying_vrsrc;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(tail_syn.source_sync_info,
				tail_syn.target_sync_info);
			tex_bars.push_back(tex_barrier);
		}

		auto depen_info = vk::DependencyInfo{};
		depen_info.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info.setImageMemoryBarriers(tex_bars);
		depen_info.setBufferMemoryBarriers(buf_bars);
		cmd_buf.pipelineBarrier2(depen_info);
	}

	void GraphicsPassNode::SyncInsertAfterDiffQ(vk::CommandBuffer cmd_buf,
		std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
	{
		//std::vector<VkSemaphoreSubmitInfo> signal_sem_submit_info_on_diff_q;

		//for (auto& diffq_tail_sync : buf_syn_infos_tail_diff_q)
		//{
		//	uint64_t signal_value = diffq_tail_sync.signal_value.value_or(1);
		//	signal_sem_submit_info.push_back(
		//		VkSemaphoreSubmitInfo{
		//			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//			.pNext = VK_NULL_HANDLE,
		//			.semaphore = diffq_tail_sync.sync_sema->GetRawSem(),
		//			.value = signal_value,
		//			.stageMask = diffq_tail_sync.target_sync_info.stage_mask,
		//			.deviceIndex = 0 }
		//	);
		//}

		//for (auto& diffq_tail_sync : tex_syn_infos_tail_diff_q)
		//{
		//	uint64_t signal_value = diffq_tail_sync.signal_value.value_or(1);
		//	signal_sem_submit_info.push_back(
		//		VkSemaphoreSubmitInfo{
		//			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		//			.pNext = VK_NULL_HANDLE,
		//			.semaphore = diffq_tail_sync.sync_sema->GetRawSem(),
		//			.value = signal_value,
		//			.stageMask = diffq_tail_sync.target_sync_info.stage_mask,
		//			.deviceIndex = 0 });
		//}

		//VkCommandBufferSubmitInfo cmd_submit_info{
		//	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		//	.pNext = VK_NULL_HANDLE,
		//	.commandBuffer = VK_NULL_HANDLE
		//};


		//auto submit_info = VkSubmitInfo2{
		//	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		//	.pNext = VK_NULL_HANDLE,
		//	.flags = Vk::NoneFlag,

		//	.waitSemaphoreInfoCount = 0u,
		//	.pWaitSemaphoreInfos = VK_NULL_HANDLE,

		//	.commandBufferInfoCount = 1u,
		//	.pCommandBufferInfos = &cmd_submit_info,

		//	.signalSemaphoreInfoCount = static_cast<uint32_t>(signal_sem_submit_info_on_diff_q.size()),
		//	.pSignalSemaphoreInfos = signal_sem_submit_info_on_diff_q.data()
		//};

		//vkQueueSubmit2(execution_queue.GetQueue(), 1, &submit_info, VK_NULL_HANDLE);


		//After waiting, we can use barriers.
		std::vector<vk::BufferMemoryBarrier2> buf_bars;
		for (auto& tail_syn : buf_syn_infos_tail_diff_q)
		{
			VirtualBuffer& buff = *tail_syn.underlying_vrsrc;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(tail_syn.source_sync_info,
				tail_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};

		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& tail_syn : tex_syn_infos_tail_diff_q)
		{
			VirtualTexture& tex = *tail_syn.underlying_vrsrc;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(tail_syn.source_sync_info,
				tail_syn.target_sync_info);
			tex_bars.push_back(tex_barrier);
		}

		auto depen_info = vk::DependencyInfo{};
		depen_info.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info.setImageMemoryBarriers(tex_bars);
		depen_info.setBufferMemoryBarriers(buf_bars);
		cmd_buf.pipelineBarrier2(depen_info);
	}

	void GraphicsPassNode::SyncInsertAhead(vk::CommandBuffer cmd_buf,
		std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info)
	{
		SyncInsertAheadSameQ(cmd_buf, wait_sem_submit_info);
		SyncInsertAheadDiffQ(cmd_buf, wait_sem_submit_info);
	}


	PassType GraphicsPassNode::GetRenderpassType()
	{
		return PassType::None;
	}

	VirtualTexture& GraphicsPassNode::GetVRsrcFromRsrcItr(const VTexItr tex_itr)
	{
		return *tex_itr;
	}

	VirtualBuffer& GraphicsPassNode::GetVRsrcFromRsrcItr(const VBufItr buf_itr)
	{

		return *buf_itr;
	}


	VBufItr GraphicsPassNode::CreateAndStoreVirtualBuffer(const std::string& underlying_vrsrc_name, std::shared_ptr<Buffer>& ptr_buf)
	{
		return rg_buffers.emplace(rg_buffers.end(), underlying_vrsrc_name, ptr_buf);
	}

	VBufItr GraphicsPassNode::CreateAndStoreVirtualBuffer(const std::string& underlying_vrsrc_name, const Buffer::Descriptor& buf_descriptor)
	{
		return rg_buffers.emplace(rg_buffers.end(), underlying_vrsrc_name, buf_descriptor);
	}

	VTexItr GraphicsPassNode::CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name, std::shared_ptr<VkTexture>& ptr_tex)
	{
		return rg_textures.emplace(rg_textures.end(), underlying_vrsrc_name, ptr_tex);
	}

	VTexItr GraphicsPassNode::CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name, const VkTexture::Descriptor& buf_descriptor)
	{
		return rg_textures.emplace(rg_textures.end(), underlying_vrsrc_name, buf_descriptor);
	}

	VTexItr GraphicsPassNode::CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name, std::vector<std::shared_ptr<VkTexture>>& model_textures)
	{
		return rg_textures.emplace(rg_textures.end(), underlying_vrsrc_name, &model_textures);
	}

	void GraphicsPassNode::AddCurPass2VRsrc(VBufItr vbuf_itr, RsrcAccessTypeRG access_t_)
	{
		vbuf_itr->passes_access_this_rsrc.emplace_back(this, access_t_);
	}

	void GraphicsPassNode::AddCurPass2VRsrc(VTexItr vtex_itr, RsrcAccessTypeRG access_t_)
	{
		vtex_itr->passes_access_this_rsrc.emplace_back(this, access_t_);
	}



	VirtualBufRsrcAndUsage& GraphicsPassNode::In(const std::string& rsrc_name, std::shared_ptr<Buffer> ptr_buf,
		BufUsage buf_usage)
	{
		//决定资源的来源，之后还需要多一个类型用来处理诸如TAA的问题
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
		const auto underlying_rsrc_name = rsrc_name + "_";

		//首先看是否已经存在于buffers map中，如果没有就首次创建。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu（有没有提供descriptor，没有就不用实体化）
		auto [iterator, inserted] = name_2_vbuf_itr.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualBuffer(underlying_rsrc_name, ptr_buf);
		}
		VBufItr underlying_rsrc_itr = iterator->second;


		//资源使用方式存储。
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, underlying_rsrc_itr);

		//检查重复
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In"; //把pass的名字作为后缀，创建导入导出口
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		//创建当前pass的资源的导入口
		ins_buf.emplace_back(vrsrc_usage, this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_itr, buf_usage.access_type);

		//[此资源不需要在当前pass实体化]
		return vrsrc_usage;
	}


	//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出.从其他pass的outlets中得到对应资源
	VirtualBufRsrcAndUsage& GraphicsPassNode::In(RsrcOutlet<VirtualBufRsrcAndUsage>& source_outlet, BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;

		//资源使用方式存储
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, source_outlet.GetUnderlyingRsrcItr());
		const auto& rsrc_name = source_outlet.GetUnderlyingRsrcItr()->name;

		//创建当前pass的资源的导入口
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);
		RsrcInlet<VirtualBufRsrcAndUsage>& cur_inlet = ins_buf.emplace_back(vrsrc_usage, this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(source_outlet.GetUnderlyingRsrcItr(), buf_usage.access_type);

		//给inlet赋值提供它的pass
		cur_inlet.AssignProvidingOutlet(source_outlet);

		//[此资源不需要在当前pass实体化]
		return vrsrc_usage;
	}



	//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
	RsrcOutlet<VirtualBufRsrcAndUsage>& GraphicsPassNode::Out(const std::string& rsrc_name, Buffer::Descriptor buf_descriptor,
		const std::function<void(Buffer::Descriptor& desco)>&
		descriptor_modifier, BufUsage buf_usage)
	{
		//当前资源含有descriptor，资源就是在当前pass创建。
		buf_usage.origin = IRsrcUsage::RsrcOrigin::EstablishedInSitu;

		assert(buf_usage.access_type == Anni::RsrcAccessTypeRG::Write);
		//资源的命名方式为：资源名 + _
		const auto underlying_rsrc_name = rsrc_name + "_";

		//用传入的lambda函数改变descriptor
		descriptor_modifier(buf_descriptor);


		//加入资源的容器中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
		//确保用户没有重复添加
		assert(!name_2_vbuf_itr.contains(underlying_rsrc_name));
		const VBufItr underlying_rsrc_itr = CreateAndStoreVirtualBuffer(underlying_rsrc_name, buf_descriptor);
		name_2_vbuf_itr.emplace(underlying_rsrc_name, underlying_rsrc_itr);



		//资源使用方式存储
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, underlying_rsrc_itr);

		//检查重复
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);


		//创建当前pass的资源的导出口
		RsrcOutlet<VirtualBufRsrcAndUsage>& cur_outlet = outs_buf.emplace_back(vrsrc_usage, this);


		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_itr, buf_usage.access_type);

		//[此资源需要在当前pass实体化]
		buf_init_list.push_back(underlying_rsrc_itr);

		return cur_outlet;
	}

	//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
	RsrcOutlet<VirtualBufRsrcAndUsage>& GraphicsPassNode::Out(const std::string& rsrc_name, std::shared_ptr<Buffer>& ptr_buf, BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;

		const auto underlying_rsrc_name = rsrc_name + "_";
		//首先看imported virtual resource是否已经存在了
		auto [iterator, inserted] = name_2_vbuf_itr.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualBuffer(underlying_rsrc_name, ptr_buf);
		}
		VBufItr underlying_rsrc_itr = iterator->second;


		//资源使用方式存储
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, underlying_rsrc_itr);

		//检查重复
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);

		//创建当前pass的资源的导出口
		RsrcOutlet<VirtualBufRsrcAndUsage>& cur_outlet = outs_buf.emplace_back(vrsrc_usage, this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_itr, buf_usage.access_type);

		//[此资源不需要在当前pass实体化]
		return cur_outlet;
	}

	//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
	RsrcOutlet<VirtualBufRsrcAndUsage>& GraphicsPassNode::Out(RsrcOutlet<VirtualBufRsrcAndUsage>& source_outlet, BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;


		//资源使用方式存储
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, source_outlet.GetUnderlyingRsrcItr());
		const auto& rsrc_name = source_outlet.GetUnderlyingRsrcItr()->name;

		//创建当前pass的资源的导出口
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Inlet Exists!");
		outlet_names.insert(cur_outlet_name);


		RsrcOutlet<VirtualBufRsrcAndUsage>& cur_outlet = outs_buf.emplace_back(vrsrc_usage, this);

		//给outlet赋值提供它的pass
		cur_outlet.AssignProvidingOutlet(source_outlet);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(source_outlet.GetUnderlyingRsrcItr(), buf_usage.access_type);

		//[此资源不需要在当前pass实体化]
		return cur_outlet;
	}













	//所有Texture相关的输入输出函数
	//************************************************************************************************
	//资源来自rendergraph之外，并且是来自模型的texture。

	VirtualTexRsrcAndUsage& GraphicsPassNode::In(const std::string& rsrc_name, std::vector<std::shared_ptr<VkTexture>>& model_textures, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		const Anni::RsrcAccessTypeRG access_type =
			std::visit([&](auto& variant_usage) -> Anni::RsrcAccessTypeRG
				{
					variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
					return variant_usage.access_type;
				}, tex_usage);

		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph

		const auto underlying_rsrc_name = rsrc_name + "_";

		//首先看是否已经存在于textures 容器中，如果没有就重新创建虚拟资源。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu（有没有提供descriptor，没有就不用实体化）
		auto [iterator, inserted] = name_2_vtex_itr.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualTexture(underlying_rsrc_name, model_textures);
		}
		VTexItr underlying_rsrc_itr = iterator->second;

		//资源使用方式存储
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);


		//检查重复
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In"; //把pass的名字作为后缀，用来创建inlet和outlet
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		//创建当前pass的资源的导入口
		ins_tex.emplace_back(vrsrc_usage, this);

		//给underlying resource增加使用它的pass，方便编译阶段处理
		AddCurPass2VRsrc(underlying_rsrc_itr, access_type);

		//[此资源不需要在当前pass实体化]
		return vrsrc_usage;
	}

	VirtualTexRsrcAndUsage& GraphicsPassNode::In(const std::string& rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
				access_type = variant_usage.access_type;
			}, tex_usage);

		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
		const auto underlying_rsrc_name = rsrc_name + "_"; //无pass后缀

		//首先看是否已经存在于textures 容器中，如果没有就重新创建虚拟资源。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu（有没有提供descriptor，没有就不用实体化）
		auto [iterator, inserted] = name_2_vtex_itr.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualTexture(underlying_rsrc_name, ptr_tex);
		}
		VTexItr underlying_rsrc_itr = iterator->second;

		//资源使用方式存储
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);

		//检查重复
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In"; //把pass的名字作为后缀，用来创建inlet和outlet
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		//创建当前pass的资源的导入口
		ins_tex.emplace_back(vrsrc_usage, this);

		//给underlying resource增加使用它的pass，方便编译阶段处理
		AddCurPass2VRsrc(underlying_rsrc_itr, access_type);

		//[此资源不需要在当前pass实体化]
		return vrsrc_usage;
	}





	VirtualTexRsrcAndUsage& GraphicsPassNode::In(RsrcOutlet<VirtualTexRsrcAndUsage>& source_outlet, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
				access_type = variant_usage.access_type;
			}, tex_usage);

		auto vrsrc_itr = source_outlet.GetUnderlyingRsrcItr();

		//资源使用方式存储
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, source_outlet.GetUnderlyingRsrcItr());
		const auto& rsrc_name = source_outlet.GetUnderlyingRsrcItr()->name;


		//创建当前pass的资源的导入口
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);
		RsrcInlet<VirtualTexRsrcAndUsage>& cur_inlet = ins_tex.emplace_back(vrsrc_usage, this);


		//给ItrInRsrcMap增加使用它的pass
		AddCurPass2VRsrc(source_outlet.GetUnderlyingRsrcItr(), access_type);


		//给inlet赋值提供它的pass
		cur_inlet.AssignProvidingOutlet(source_outlet);

		//[此资源不需要在当前pass实体化]
		return vrsrc_usage;
	}

	RsrcOutlet<VirtualTexRsrcAndUsage>& GraphicsPassNode::Out(const std::string& rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier, std::variant<TexUsage, AttachUsage> tex_usage)
	{

		Anni::RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::EstablishedInSitu;
				access_type = variant_usage.access_type;
			}, tex_usage);
		assert(access_type == Anni::RsrcAccessTypeRG::Write);

		//资源的命名方式为：资源名 + _
		const auto underlying_rsrc_name = rsrc_name + "_";

		//用传入的lambda函数改变descriptor
		descriptor_modifier(tex_descriptor);

		//加入资源的容器中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
		//确保用户没有重复添加
		assert(!name_2_vtex_itr.contains(underlying_rsrc_name));
		const VTexItr underlying_rsrc_itr = CreateAndStoreVirtualTexture(underlying_rsrc_name, tex_descriptor);
		name_2_vtex_itr.emplace(underlying_rsrc_name, underlying_rsrc_itr);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_itr, access_type);

		//资源使用方式存储
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);

		//创建当前pass的资源的导出口
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);
		RsrcOutlet<VirtualTexRsrcAndUsage>& cur_outlet = outs_tex.emplace_back(vrsrc_usage, this);

		//[此资源需要在当前pass实体化]
		tex_init_list.push_back(underlying_rsrc_itr);

		return cur_outlet;
	}



	RsrcOutlet<VirtualTexRsrcAndUsage>& GraphicsPassNode::Out(const std::string& rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
				access_type = variant_usage.access_type;
			}, tex_usage);

		const auto underlying_rsrc_name = rsrc_name + "_";
		//首先看imported virtual resource是否已经存在了
		auto [iterator, inserted] = name_2_vtex_itr.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualTexture(underlying_rsrc_name, ptr_tex);
		}
		VTexItr underlying_rsrc_itr = iterator->second;


		//资源使用方式存储
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_itr, access_type);

		//创建当前pass的资源的导出口
		//检查重复
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);
		auto& cur_outlet = outs_tex.emplace_back(vrsrc_usage, this);

		//[此资源不需要在当前pass实体化]
		return cur_outlet;
	}


	RsrcOutlet<VirtualTexRsrcAndUsage>& GraphicsPassNode::Out(RsrcOutlet<VirtualTexRsrcAndUsage>& source_outlet, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
				access_type = variant_usage.access_type;
			}, tex_usage);

		//资源使用方式存储
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, source_outlet.GetUnderlyingRsrcItr());
		const auto& rsrc_name = source_outlet.GetUnderlyingRsrcItr()->name;

		//创建当前pass的资源的导出口
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Inlet Exists!");
		outlet_names.insert(cur_outlet_name);
		RsrcOutlet<VirtualTexRsrcAndUsage>& cur_outlet = outs_tex.emplace_back(vrsrc_usage, this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(source_outlet.GetUnderlyingRsrcItr(), access_type);

		//给outlet赋值提供它的pass
		cur_outlet.AssignProvidingOutlet(source_outlet);


		//[此资源不需要在当前pass实体化]
		return cur_outlet;
	}
}
