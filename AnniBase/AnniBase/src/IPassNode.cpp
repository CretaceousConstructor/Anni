#include "IPassNode.h"
namespace Anni::RenderGraphV1
{
	GraphicsPassNode::GraphicsPassNode(std::string name_, DeviceManager& device_manager_, SwapchainManager& swapchain_manager_, DescriptorLayoutManager& descriptor_set_layout_manager_, VkShaderFactory& shader_fac_, DescriptorSetAllocatorGrowable& descriptor_allocator_, std::unordered_map<std::string, VirtualBuffer>& rg_buffers_map_, std::unordered_map<std::string, VirtualTexture>& rg_textures_map_) :
		name(name_),
		device_manager(device_manager_),
		swapchain_manager(swapchain_manager_),

		descriptor_set_layout_manager(descriptor_set_layout_manager_),
		shader_fac(shader_fac_),
		descriptor_allocator(descriptor_allocator_),
		rg_buffers_map(rg_buffers_map_),
		rg_textures_map(rg_textures_map_)
	{
	}





	void GraphicsPassNode::InsertSyncInfoForInitalUsage(
		const Anni::BufSyncInfo& source_syn_info,
		const Anni::BufSyncInfo& target_syn_info,
		const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc)
	{
		buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}

	void GraphicsPassNode::InsertSyncInfoForInitalUsage(
		const Anni::ImgSyncInfo& source_syn_info,
		const Anni::ImgSyncInfo& target_syn_info,
		const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc)
	{
		tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}





	void GraphicsPassNode::InsertSameQueueSyncInfo(
		GraphicsPassNode* const source_pass,
		GraphicsPassNode* const target_pass,
		Queue* const queue_,
		const Anni::BufSyncInfo& source_syn_info,
		const Anni::BufSyncInfo& target_syn_info,
		const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc)
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
		const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc)
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
		const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc,

		SemInsertInfoSafe sor_sem_insersion_info,
		SemInsertInfoSafe tar_sem_insersion_info)
	{
		if (this == source_pass)
		{
			buf_syn_infos_tail_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc);

			const auto key = PassNodePair{ source_pass,target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), sor_sem_insersion_info));
			}
			else
			{
				pass_pair_to_t_sem.at(key).second.signal_stages |= sor_sem_insersion_info.signal_stages;
			}
		}
		else if (this == target_pass)
		{
			buf_syn_infos_head_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc);

			const auto key = PassNodePair{ source_pass,target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), tar_sem_insersion_info));
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
		const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc,

		SemInsertInfoSafe sor_sem_insersion_info,
		SemInsertInfoSafe tar_sem_insersion_info)
	{
		if (this == source_pass)
		{
			tex_syn_infos_tail_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc);

			const auto key = PassNodePair{ source_pass,target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), sor_sem_insersion_info));
			}
			else
			{
				pass_pair_to_t_sem.at(key).second.signal_stages |= sor_sem_insersion_info.signal_stages;
			}
		}
		else if (this == target_pass)
		{
			tex_syn_infos_head_diff_q.emplace_back( source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc);

			const auto key = PassNodePair{ source_pass,target_pass };

			if (!pass_pair_to_t_sem.contains(key))
			{
				pass_pair_to_t_sem.emplace(key, std::pair<vk::Semaphore, SemInsertInfoSafe>(sema_sync->GetRaw(), tar_sem_insersion_info));
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
		////REQUIRED RESOURCES INITIALIZATION
		for (auto& buf_itr : buf_init_list)
		{
			//VirtualResource has a descriptor to help initalize resources
			buf_fac.ActualizeVirtualResource(buf_itr->second);
		}

		for (auto& tex_itr : tex_init_list)
		{
			//VirtualResource has a descriptor to help initalize resources
			tex_fac.ActualizeVirtualResource(tex_itr->second);
		}
	}

	inline void GraphicsPassNode::SyncInsertAfter(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
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




	void GraphicsPassNode::SyncInsertAheadSameQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info)
	{

		//wait for semaphores(even in the same queue��some resources are being loaded, thus needs to wait for semaphores' signal first).

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
			VirtualBuffer& buff = head_syn.underlying_vrsrc->second;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};


		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& head_syn : tex_syn_infos_head_same_q)
		{
			VirtualTexture& tex = head_syn.underlying_vrsrc->second;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
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


	void GraphicsPassNode::SyncInsertAheadDiffQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info)
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
			VirtualBuffer& buff = head_syn.underlying_vrsrc->second;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};


		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& head_syn : tex_syn_infos_head_diff_q)
		{
			VirtualTexture& tex = head_syn.underlying_vrsrc->second;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
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

	void GraphicsPassNode::SyncInsertAfterSameQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
	{
		//we can use barriers.
		//OPTIMIZATION: ����

		//After waiting, we can use barriers.
		std::vector<vk::BufferMemoryBarrier2> buf_bars;
		for (auto& tail_syn : buf_syn_infos_tail_same_q)
		{
			VirtualBuffer& buff = tail_syn.underlying_vrsrc->second;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(tail_syn.source_sync_info, tail_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};

		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& tail_syn : tex_syn_infos_tail_same_q)
		{
			VirtualTexture& tex = tail_syn.underlying_vrsrc->second;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(tail_syn.source_sync_info, tail_syn.target_sync_info);
			tex_bars.push_back(tex_barrier);
		}

		auto depen_info = vk::DependencyInfo{};
		depen_info.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info.setImageMemoryBarriers(tex_bars);
		depen_info.setBufferMemoryBarriers(buf_bars);
		cmd_buf.pipelineBarrier2(depen_info);
	}

	void GraphicsPassNode::SyncInsertAfterDiffQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
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
			VirtualBuffer& buff = tail_syn.underlying_vrsrc->second;
			vk::BufferMemoryBarrier2 buf_barrier = buff.GetBufBarrier(tail_syn.source_sync_info, tail_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		};

		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& tail_syn : tex_syn_infos_tail_diff_q)
		{
			VirtualTexture& tex = tail_syn.underlying_vrsrc->second;
			vk::ImageMemoryBarrier2 tex_barrier = tex.GetTexBarrier(tail_syn.source_sync_info, tail_syn.target_sync_info);
			tex_bars.push_back(tex_barrier);
		}

		auto depen_info = vk::DependencyInfo{};
		depen_info.dependencyFlags = vk::DependencyFlagBits::eByRegion;
		depen_info.setImageMemoryBarriers(tex_bars);
		depen_info.setBufferMemoryBarriers(buf_bars);
		cmd_buf.pipelineBarrier2(depen_info);

	}

	void GraphicsPassNode::SyncInsertAhead(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info)
	{
		SyncInsertAheadSameQ(cmd_buf, wait_sem_submit_info);
		SyncInsertAheadDiffQ(cmd_buf, wait_sem_submit_info);
	}





	PassType GraphicsPassNode::GetRenderpassType()
	{
		return PassType::None;
	}

	VirtualBufRsrcAndUsage& GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, BufUsage buf_usage)
	{
		//������Դ����Դ��֮����Ҫ��һ������������������TAA������
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
		const auto underlying_rsrc_name = rsrc_name + std::string("_");

		//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о��״δ�����ע�����ﴴ������������Դ�������費��Ҫʵ�廯��Ҫ���ǲ���EstablishedInSitu
		auto rsrc_itr = rg_buffers_map.find(underlying_rsrc_name);
		if (rsrc_itr == rg_buffers_map.end())
		{
			const auto temp_itr = rg_buffers_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_buf);        //��ֵ��emplace����Ԫ������Ĳ���
			rsrc_itr = temp_itr.first;
		}

		//��underlying resource����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage.access_type);

		//��Դʹ�÷�ʽ�洢��
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, rsrc_itr);

		const auto inlet_name = rsrc_name + "_" + this->name + "_In";        //��pass��������Ϊ��׺���������뵼����

		//������ǰpass����Դ�ĵ����
		const auto cur_inlet_itr = ins_buf.try_emplace(inlet_name, vrsrc_usage, this);
		ASSERT_WITH_MSG(cur_inlet_itr.second, "Same Inlet Exists!");

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	//����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
	VirtualBufRsrcAndUsage& GraphicsPassNode::In(const std::string rsrc_name, GraphicsPassNode& source_pass, BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
		const auto source_outlet_name = rsrc_name + "_" + source_pass.name + "_Out";        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
		const auto source_outlet_itr = source_pass.outs_buf.find(source_outlet_name);
		assert(source_pass.outs_buf.end() != source_outlet_itr);        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ��ֱ��crash


		//��ItrInRsrcMap ����ʹ������pass
		source_outlet_itr->second.GetUnderlyingRsrcItr()->second.passes_access_this_rsrc.emplace_back(this, buf_usage.access_type);


		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, source_outlet_itr->second.GetUnderlyingRsrcItr());


		//������ǰpass����Դ�ĵ����
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";
		const auto cur_inlet_itr = ins_buf.try_emplace(cur_inlet_name, vrsrc_usage, this);

		assert(cur_inlet_itr.second);

		//��source outlet����ʹ������pass���Լ�ʹ������pass��Ӧ��ʹ�÷�ʽ
		//source_outlet_itr->second.AddAccessingPassAndItr(this, cur_inlet_itr.first);

		//��inlet��ֵ�ṩ����pass
		cur_inlet_itr.first->second.AssignProvidingPassAndProvidingOutItr(&source_pass, source_outlet_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	//buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�

	VirtualBufRsrcAndUsage& GraphicsPassNode::Out(const std::string rsrc_name, Buffer::Descriptor buf_descriptor, const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier, BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::EstablishedInSitu;
		//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������

		//��Դ��������ʽΪ����Դ�� + _
		const auto rsrc_name_inmap = rsrc_name + "_";

		//�ô����lambda�����ı�descriptor
		descriptor_modifier(buf_descriptor);

		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
		const auto underlying_rsrc_itr = rg_buffers_map.try_emplace(rsrc_name_inmap, rsrc_name_inmap, buf_descriptor);
		//ȷ���û�û���ظ����
		assert(underlying_rsrc_itr.second);

		//��underlying resource����ʹ������pass
		underlying_rsrc_itr.first->second.passes_access_this_rsrc.emplace_back(this, buf_usage.access_type);

		assert(buf_usage.access_type == Anni::RsrcAccessTypeRG::Write);
		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, underlying_rsrc_itr.first);



		//������ǰpass����Դ�ĵ�����
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		const auto cur_out_itr = outs_buf.try_emplace(cur_outlet_name, vrsrc_usage, this);

		assert(cur_out_itr.second);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		buf_init_list.push_back(underlying_rsrc_itr.first);

		return vrsrc_usage;
	}

	//buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��

	VirtualBufRsrcAndUsage& GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;

		const auto underlying_rsrc_name = rsrc_name + "_";

		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
		auto underlying_rsrc_itr = rg_buffers_map.find(underlying_rsrc_name);
		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

		//���virtual resource�����ڣ��򴴽�:
		if (underlying_rsrc_itr == rg_buffers_map.end())
		{
			const auto temp_itr = rg_buffers_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_buf);        //��ֵ��emplace��������Ĳ���
			underlying_rsrc_itr = temp_itr.first;
		}

		//��underlying resource����ʹ������pass
		underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage.access_type);

		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, underlying_rsrc_itr);


		//������ǰpass����Դ�ĵ�����
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		const auto outlet_itr = outs_buf.try_emplace(cur_outlet_name, vrsrc_usage, this);

		assert(outlet_itr.second);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	//����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��

	VirtualBufRsrcAndUsage& GraphicsPassNode::Out(const std::string rsrc_name, GraphicsPassNode& source_pass, BufUsage buf_usage)
	{

		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
		const auto source_outlet_name = rsrc_name + "_" + source_pass.name + "_Out";

		//����һ��  Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
		const auto source_outlet_itr = source_pass.outs_buf.find(source_outlet_name);
		assert(source_pass.outs_buf.end() != source_outlet_itr);        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ

		//��ItrInRsrcMap����ʹ������pass
		source_outlet_itr->second.GetUnderlyingRsrcItr()->second.passes_access_this_rsrc.emplace_back(this, buf_usage.access_type);

		//����Դʹ�÷�ʽ�洢����
		auto& vrsrc_usage = this->buf_usages.emplace_back(buf_usage, source_outlet_itr->second.GetUnderlyingRsrcItr());

		//������ǰpass����Դ�ĵ�����
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		const auto cur_outlet_itr = outs_buf.try_emplace(cur_outlet_name, vrsrc_usage, this);

		assert(cur_outlet_itr.second);

		//source pass�е�outlet���accessing pass
		//source_outlet_itr->second.AddAccessingPassAndItr(this, cur_outlet_itr.first);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}










	//����Texture��ص������������
	//************************************************************************************************
	//��Դ����rendergraph֮�⣬����������ģ�͵�texture��

	VirtualTexRsrcAndUsage& GraphicsPassNode::In(const std::string rsrc_name, std::vector<std::shared_ptr<VkTexture>>& model_textures, std::variant<TexUsage, AttachUsage> tex_usage)
	{

		Anni::RsrcAccessTypeRG asscess_type =
			std::visit([&](auto& variant_usage) ->Anni::RsrcAccessTypeRG
				{
					variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
					return variant_usage.access_type;
				}, tex_usage);

		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph

		const auto underlying_rsrc_name = rsrc_name + "_";
		//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���������Դ
		auto underlying_rsrc_itr = rg_textures_map.find(underlying_rsrc_name);

		if (underlying_rsrc_itr == rg_textures_map.end())
		{
			const auto temp_itr = rg_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, &model_textures);        //��ֵ��emplace����Ԫ������Ĳ���
			underlying_rsrc_itr = temp_itr.first;
		}

		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);

		//��ItrInRsrcMap����ʹ������pass���������׶δ���
		underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, asscess_type);

		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";        //��pass��������Ϊ��׺����������inlet��outlet
		//������ǰpass����Դ�ĵ����
		const auto cur_inlet_in_itr = ins_tex.try_emplace(cur_inlet_name, vrsrc_usage, this);
		assert(cur_inlet_in_itr.second);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	VirtualTexRsrcAndUsage& GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG asscess_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
				asscess_type = variant_usage.access_type;
			}, tex_usage);

		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
		const auto underlying_rsrc_name = rsrc_name + "_";        //��pass��׺

		//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���
		auto underlying_rsrc_itr = rg_textures_map.find(underlying_rsrc_name);
		if (underlying_rsrc_itr == rg_textures_map.end())
		{
			const auto temp_itr = rg_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
			underlying_rsrc_itr = temp_itr.first;
		}

		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";        //��pass��������Ϊ��׺����������inlet��outlet


		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);

		//������ǰpass����Դ�ĵ����
		const auto cur_inlet_in_itr = ins_tex.try_emplace(cur_inlet_name, vrsrc_usage, this);
		assert(cur_inlet_in_itr.second);

		//��ItrInRsrcMap����ʹ������pass���������׶δ���
		underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, asscess_type);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	VirtualTexRsrcAndUsage& GraphicsPassNode::In(const std::string rsrc_name, GraphicsPassNode& source_pass, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG asscess_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
				asscess_type = variant_usage.access_type;

			}, tex_usage);



		const auto source_outlet_name = rsrc_name + "_" + source_pass.name + "_Out";        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
		const auto source_outlet_itr = source_pass.outs_tex.find(source_outlet_name);        //��GbufferPos_GPass_Out
		assert(source_pass.outs_tex.end() != source_outlet_itr);        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ

		auto vrsrc_itr = source_outlet_itr->second.GetUnderlyingRsrcItr();


		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, vrsrc_itr);

		//��ItrInRsrcMap����ʹ������pass
		vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, asscess_type);

		//������ǰpass����Դ�ĵ����
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";
		const auto cur_inlet_itr = ins_tex.try_emplace(cur_inlet_name, vrsrc_usage, this);

		assert(cur_inlet_itr.second);

		//��source outlet����ʹ������pass
		//source_outlet_itr->second.AddAccessingPassAndItr(this, cur_inlet_itr.first);

		//��inlet��ֵ�ṩ����pass
		cur_inlet_itr.first->second.AssignProvidingPassAndProvidingOutItr(&source_pass, source_outlet_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	VirtualTexRsrcAndUsage& GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier, std::variant<TexUsage, AttachUsage> tex_usage)
	{
		Anni::RsrcAccessTypeRG asscess_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::EstablishedInSitu;
				asscess_type = variant_usage.access_type;
			}, tex_usage);

		//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
		const auto underlying_rsrc_name = rsrc_name + "_";

		//�ô����lambda�����ı�descriptor
		descriptor_modifier(tex_descriptor);

		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
		const auto underlying_rsrc_itr = rg_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, tex_descriptor);
		//ȷ���û�û���ظ����
		assert(underlying_rsrc_itr.second);

		//��underlying resource����ʹ������pass
		underlying_rsrc_itr.first->second.passes_access_this_rsrc.emplace_back(this, asscess_type);

		assert(asscess_type == Anni::RsrcAccessTypeRG::Write);

		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr.first);
		//������ǰpass����Դ�ĵ�����
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";

		const auto cur_outlet_itr = outs_tex.try_emplace(cur_outlet_name, vrsrc_usage, this);

		assert(cur_outlet_itr.second);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		tex_init_list.push_back(underlying_rsrc_itr.first);

		return vrsrc_usage;
	}

	VirtualTexRsrcAndUsage& GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage)
	{

		Anni::RsrcAccessTypeRG asscess_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
				asscess_type = variant_usage.access_type;
			}, tex_usage);

		const auto underlying_rsrc_name = rsrc_name + std::string("_");

		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
		auto underlying_rsrc_itr = rg_textures_map.find(underlying_rsrc_name);
		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

		//���virtual resource�����ڣ��򴴽�:
		if (underlying_rsrc_itr == rg_textures_map.end())
		{
			const auto temp_itr = rg_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //��ֵ��emplace��������Ĳ���
			underlying_rsrc_itr = temp_itr.first;
		}

		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, underlying_rsrc_itr);

		//��ItrInRsrcMap����ʹ������pass
		underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, asscess_type);

		//������ǰpass����Դ�ĵ�����
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		const auto cur_outlet_itr = outs_tex.try_emplace(cur_outlet_name, vrsrc_usage, this);

		assert(cur_outlet_itr.second);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

	VirtualTexRsrcAndUsage& GraphicsPassNode::Out(const std::string rsrc_name, GraphicsPassNode& source_pass, std::variant<TexUsage, AttachUsage> tex_usage)
	{

		Anni::RsrcAccessTypeRG asscess_type;
		std::visit([&](auto& variant_usage)
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
				asscess_type = variant_usage.access_type;
			}, tex_usage);

		const auto source_outlet_name = rsrc_name + source_pass.name + "_Out";

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
		const auto source_outlet_itr = source_pass.outs_tex.find(source_outlet_name);        //��GbufferPos_GPass_Out

		//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
		assert(source_pass.outs_tex.end() != source_outlet_itr);        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		auto vrsrc_itr = source_outlet_itr->second.GetUnderlyingRsrcItr();
		//��ItrInRsrcMap����ʹ������pass
		vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, asscess_type);

		//��Դʹ�÷�ʽ�洢
		auto& vrsrc_usage = this->tex_usages.emplace_back(tex_usage, vrsrc_itr);

		//������ǰpass����Դ�ĵ�����
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		const auto cur_out_itr = outs_tex.try_emplace(cur_outlet_name, vrsrc_usage, this);

		assert(cur_out_itr.second);

		//��source outlet����ʹ������pass
		//source_outlet_itr->second.AddAccessingPassAndItr(this, cur_out_itr.first);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return vrsrc_usage;
	}

}
