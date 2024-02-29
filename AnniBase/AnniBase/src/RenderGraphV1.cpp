#include "RenderGraphV1.h"


Anni::RenderGraphV1::DependencyGraph::DependencyGraph(DeviceManager& device_manager_,
	SwapchainManager& swapchain_manager_,
	QueueManager& queue_manager_,
	DescriptorLayoutManager& descriptor_set_layout_manager_,
	DescriptorSetAllocatorGrowable& descriptor_allocator_,
	VkTimelineSemPoolUnsafe& semaphore_pool_, BufferFactory& buf_fac_,
	VkTextureFactory& tex_fac_, VkShaderFactory& shader_fac_,
	GFXPipelineCIBuilder& gfx_pipelineCI_builder_,
	VkPipelineBuilder& pipeline_builder_
)
	:
	cur_frame(0),
	img_index(0),
	device_manager(device_manager_),
	swapchain_manager(swapchain_manager_),
	queue_manager(queue_manager_),
	descriptor_set_layout_manager(descriptor_set_layout_manager_),
	descriptor_allocator(descriptor_allocator_),
	semaphore_pool(semaphore_pool_),
	buf_fac(buf_fac_),
	tex_fac(tex_fac_),
	shader_fac(shader_fac_),
	gfx_pipelineCI_builder(gfx_pipelineCI_builder_),
	pipeline_builder(pipeline_builder_)

{
}

void Anni::RenderGraphV1::DependencyGraph::SetCurFrameAndImgIndex(uint64_t cur_frame_, uint32_t img_index_)
{
	cur_frame = cur_frame_;
	img_index = img_index_;
}


void Anni::RenderGraphV1::DependencyGraph::Compile()
{
	//TODO: Trim all invalid passes and unused resources
	//->Render graph compilation starts.
	//************************************
	topologically_sorted_nodes.clear(); //清除最后的执行顺序。 
	GeneratePassNodeDepen(); //根据In,Out函数声明确定可以推断出来的顺序。
	TopologicalDFSSort(); //排序
	DependencyLevelsGeneration();
	GPUWorkDistribution();
	SyncPrimitivesAquisition();
	MarkLayersMultiAccessSameRsrc();
	PartitionPassesAtTheSameLevel();
	SortPassesAccessingRsrc();
	SyncPrimitivesInsertion();
	//************************************
	//<-Render graph compilation finished.
}

void Anni::RenderGraphV1::DependencyGraph::CmdBufRecordingAndExecuting(uint32_t img_index, uint32_t cur_frame,
	vk::Semaphore frame_number_semaphore)
{
	//TODO: PARALLEL RECORDING MULTI CMD BUF. Should be done in a per-queue basis? You can't fill command buffers created by the same pool from multi threads, now we just seqentially record.
	for (size_t level = 0; level <= max_level; ++level)
	{
		for (auto& ptr_pass : topologically_sorted_nodes)
		{
			if (ptr_pass->cur_level == level)
			{
				if (level_to_all_passes_attached.contains(level))
				{
					level_to_all_passes_attached[level].push_back(ptr_pass);
				}
				level_to_all_passes_attached.emplace(level, std::vector<GraphicsPassNode*>{});
			}
		}
	}


	//{
	//	auto p_random_q = queue_to_passes_on_it.begin()->first;
	//	//helper of sync of swap chain image
	//	for (auto& tex : rg_textures)
	//	{
	//		auto& p_rsrc = tex.second.p_rsrc;
	//		if (p_rsrc->IsSwapTexture())
	//		{
	//			{
	//				//std::pair<VkCommandBuffer, VkSemaphore> cmd_buf_and_sem = p_random_q->GetOneBufAndAssociatedSem();

	//				//wait and signal semaphore
	//				//vk::SemaphoreSubmitInfo wait_sem_submit_info{};
	//				//wait_sem_submit_info.semaphore = p_rsrc->swap_img_rdy_4_rendering_helper_sem->GetRaw();
	//				//wait_sem_submit_info.value = p_rsrc->swap_img_rdy_4_rendering_helper_sem->GetLastValue()++;
	//				//wait_sem_submit_info.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput; //TODO: this shit need some accumulated stages

	//				//vk::SemaphoreSubmitInfo signal_sem_submit_info{};
	//				//signal_sem_submit_info.semaphore = p_rsrc->swap_img_rdy_4_rendering_helper_sem->GetRaw();
	//				//signal_sem_submit_info.value = p_rsrc->swap_img_rdy_4_rendering_helper_sem->GetLastValue();
	//				//signal_sem_submit_info.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput; //TODO: this shit need some accumulated stages

	//				//vk::SubmitInfo2 submit_info{};
	//				//submit_info.setWaitSemaphoreInfos(wait_sem_submit_info);
	//				////submit_info.setSignalSemaphoreInfos(signal_sem_submit_info);

	//				//p_random_q->GetQueue().submit2(submit_info);
	//			}

	//			{
	//				//std::vector<vk::SemaphoreSubmitInfo> wait_sem_submit_infos;

	//				//std::ranges::transform(
	//				//	swap_imgs_finshed_using_by_cur_pass,
	//				//	std::back_inserter(wait_sem_submit_infos),
	//				//	[](std::shared_ptr<BinarySemWrapper>& sem)
	//				//	{
	//				//		//wait and signal semaphore
	//				//		uint64_t wait_value = 1;
	//				//		return VkSemaphoreSubmitInfo{
	//				//				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
	//				//				.pNext = VK_NULL_HANDLE,
	//				//				.semaphore = sem->GetRaw(),
	//				//				.value = wait_value,
	//				//				.stageMask = 0,
	//				//				.deviceIndex = 0
	//				//		};
	//				//	}
	//				//);

	//				//constexpr uint64_t signal_value = 1;

	//				//VkSemaphoreSubmitInfo signal_sem_submit_info =
	//				//	VkSemaphoreSubmitInfo{
	//				//		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
	//				//		.pNext = VK_NULL_HANDLE,
	//				//		.semaphore = p_rsrc->swap_img_rdy_4_rendering_sem->GetRaw(),
	//				//		.value = signal_value,
	//				//		.stageMask = 0,
	//				//		.deviceIndex = 0
	//				//};

	//				//auto submit_info = VkSubmitInfo2{
	//				//	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
	//				//	.pNext = VK_NULL_HANDLE,
	//				//	.flags = Vk::NoneFlag,

	//				//	.waitSemaphoreInfoCount = static_cast<uint32_t>(wait_sem_submit_infos.size()),
	//				//	.pWaitSemaphoreInfos = wait_sem_submit_infos.data(),

	//				//	.commandBufferInfoCount = 0,
	//				//	.pCommandBufferInfos = VK_NULL_HANDLE,

	//				//	.signalSemaphoreInfoCount = 1,
	//				//	.pSignalSemaphoreInfos = &signal_sem_submit_info,
	//				//};

	//				//vkQueueSubmit2(p_random_q->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
	//			}

	//		}

	//	}
	//}

	for (auto level = 0; level <= max_level; ++level)
	{
		//先观测这个level上的pass需不需要排序，如果需要，那么先要决定此level的执行顺序，这个执行顺序首先根据queue fam划分，再根据queue地址划分，最后根据pass地址划分

		//不需要level内部划分的情况
		if (!if_level_accesses_same_rsrc_multimes[level])
		{
			for (auto p_pass : level_to_all_passes_attached[level])
			{
				//PRESENT PASS SPECIAL TEATMENT
				if (Present == p_pass->GetRenderpassType())
				{
					p_pass->ResourcesAcquisition(tex_fac, buf_fac);

					std::vector<vk::SemaphoreSubmitInfo> wait_sem_submit_info;
					std::vector<vk::SemaphoreSubmitInfo> signal_sem_submit_info;

					Queue& cur_present_pass_queue = *pass_to_queue_info.at(p_pass).first;
					std::pair<vk::CommandBuffer, vk::Semaphore> cmd_buf_and_sem = cur_present_pass_queue.
						GetOneBufAndAssociatedSem();

					p_pass->SyncInsertAhead(cmd_buf_and_sem.first, wait_sem_submit_info);
					//p_pass->RecordCommandBuffer(cmd_buf_and_sem.first);
					p_pass->SyncInsertAfter(cmd_buf_and_sem.first, signal_sem_submit_info);


					ASSERT_WITH_MSG(1 == p_pass->outs_tex.size(),
						"Present pass can only have swap image as its only resource.");
					auto& swap_img_usage = p_pass->outs_tex.front();
					auto& swap_img = p_pass->GetVRsrcFromRsrcHandle(swap_img_usage.rsrc_handle).p_rsrc;


					ASSERT_WITH_MSG(swap_img->IsSwapTexture(),
						"Present pass can only have swap image as its only resource.");

					{
						for (const auto& swap_img_finshed_using : vec_swap_imgs_finshed_using.back().first) //
						{
							constexpr uint64_t wait_value = 1;
							wait_sem_submit_info.emplace_back
							(
								swap_img_finshed_using->GetRaw(),
								wait_value,
								vk::PipelineStageFlagBits2::eAllCommands
							);
						}
					}


					{
						auto& rdy_4_ren_helper_sem = swap_img->swap_img_rdy_4_rendering_helper_sem; // timeline sem
						uint64_t signal_val = ++rdy_4_ren_helper_sem->GetLastValue(); //1

						signal_sem_submit_info.push_back(
							vk::SemaphoreSubmitInfo(
								rdy_4_ren_helper_sem->GetRaw(),
								signal_val,
								vk::PipelineStageFlagBits2::eAllCommands //
							)
						);

						auto& present_finished_helper_sem = swap_img->present_finished_helper_sem; //timeline sem
						signal_val = ++present_finished_helper_sem->GetLastValue(); //1

						signal_sem_submit_info.push_back(
							vk::SemaphoreSubmitInfo(
								present_finished_helper_sem->GetRaw(),
								signal_val,
								vk::PipelineStageFlagBits2::eAllCommands //
							)
						);
					}


					{
						const uint64_t wait_value = cur_frame < (Vk::MAX_FRAMES_OVERLAP - 1)
							? 0
							: cur_frame - (Vk::MAX_FRAMES_OVERLAP - 1);
						//binary sem需要用VkWaitTimelineSemaphore来防止多个等待
						vk::SemaphoreWaitInfo wait_info;
						wait_info
							.setSemaphores(frame_number_semaphore)
							.setValues(wait_value);
						const auto sem_wait_result = device_manager.GetLogicalDevice().waitSemaphores(
							wait_info, UINT64_MAX);

						ASSERT_WITH_MSG(sem_wait_result == vk::Result::eSuccess, "Waiting for semaphore failed");

						if (cur_frame >= Vk::MAX_FRAMES_OVERLAP)
						{
							// Find the first element satisfying the condition
							auto it = std::ranges::find_if(vec_swap_imgs_finshed_using,
								[&cur_frame](
									std::pair<std::vector<std::shared_ptr<BinarySemWrapper>>,
									uint64_t>& sems_and_frame_num)
								{
									return sems_and_frame_num.second == (cur_frame -
										Vk::MAX_FRAMES_OVERLAP);
								}
							);

							assert(it != vec_swap_imgs_finshed_using.end());
							// Erase the found element
							vec_swap_imgs_finshed_using.erase(it);
						}

						auto& present_finished_sem = swap_img->present_finished_sem;
						//binary sem需要用VkWaitTimelineSemaphore来防止多个等待 
						constexpr uint64_t wait_val = 1;
						wait_sem_submit_info.push_back(
							vk::SemaphoreSubmitInfo(
								present_finished_sem->GetRaw(),
								wait_val,
								vk::PipelineStageFlagBits2::eAllCommands
							)
						);

						constexpr uint64_t signal_value = 1; //binary sem需要用VkWaitTimelineSemaphore来防止多个等待
						signal_sem_submit_info.push_back(
							vk::SemaphoreSubmitInfo(
								swap_img->ready_for_present_sem->GetRaw(),
								signal_value,
								vk::PipelineStageFlagBits2::eAllCommands //
							)
						);
					}


					vk::CommandBufferSubmitInfo cmd_submit_info(cmd_buf_and_sem.first);


					vk::SubmitInfo2 submit_info{};
					submit_info
						.setCommandBufferInfos(cmd_submit_info)
						.setWaitSemaphoreInfos(wait_sem_submit_info)
						.setSignalSemaphoreInfos(signal_sem_submit_info);

					cur_present_pass_queue.GetQueue().submit2(submit_info);


					vk::PresentInfoKHR present_info{};
					present_info.setWaitSemaphores(swap_img->ready_for_present_sem->GetRaw());
					present_info.setSwapchains(swapchain_manager.GetSwapChain());
					present_info.setImageIndices(swap_img->GetSwapImageIndex());

					vk::Result present_result = cur_present_pass_queue.GetQueue().presentKHR(present_info);
					ASSERT_WITH_MSG(present_result == vk::Result::eSuccess, "failed to present swap chain image!");
					//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
					//{
					//	// TODO: recreation of swapchain
					//	// recreateSwapChain();
					//	return;
					//}
				}
				else
				{
					p_pass->ResourcesAcquisition(tex_fac, buf_fac);
					std::vector<vk::SemaphoreSubmitInfo> wait_sem_submit_info;
					std::vector<vk::SemaphoreSubmitInfo> signal_sem_submit_info;

					Queue& cur_pass_queue = *pass_to_queue_info.at(p_pass).first;
					std::pair<VkCommandBuffer, VkSemaphore> cmd_buf_and_sem = cur_pass_queue.
						GetOneBufAndAssociatedSem();

					p_pass->SyncInsertAhead(cmd_buf_and_sem.first, wait_sem_submit_info);
					//TODO: sampler and view change. this can't be done at the resource level,cause command buffers can be recoured parallelly
					p_pass->CreateDescriptorSetLayout();
					p_pass->AllocateDescriptorSets();
					p_pass->UpdateDescriptorSets();
					p_pass->GetShaders();
					p_pass->BeginRenderPass(cmd_buf_and_sem.first);
					p_pass->RecordCommandBuffer(cmd_buf_and_sem.first);
					p_pass->EndRenderPass(cmd_buf_and_sem.first);

					p_pass->SyncInsertAfter(cmd_buf_and_sem.first, signal_sem_submit_info);

					CmdBufExecuteOnQueue(p_pass, cur_frame, cmd_buf_and_sem.first, &cur_pass_queue,
						frame_number_semaphore, wait_sem_submit_info, signal_sem_submit_info);
				}
			}
		}
		else
		{
			ASSERT_WITH_MSG(false, "not yet implemented");

			//for (auto& ptr_pass : topologically_sorted_nodes)
			//{
			//	if (ptr_pass->cur_pass_level == level)
			//	{
			//		auto                                    ptr_queue = pass_to_queue_info.at(ptr_pass).first;
			//		std::pair<VkCommandBuffer, VkSemaphore> cmd_buf_and_sem = ptr_queue->GetOneBufAndAssociatedSem();
			//		//TODO: sampler and view change
			//		//ptr_pass->SyncInsertAhead(cmd_buf_and_sem.first);
			//		//ptr_pass->PreExecute();        //
			//		//ptr_pass->Execute(cmd_buf_and_sem.first);
			//		//ptr_pass->SyncInsertAfter(cmd_buf_and_sem.first);

			//		//CmdBufExecuteOnQueue(ptr_pass, cmd_buf_and_sem.first, ptr_queue, image_available_semaphore);
			//	}
			//}
		}
	}

	//Harvests all Timeline Semaphores across all queues
	std::vector<vk::Semaphore> all_waiting_queue_sems;

	for (auto& [p_queue, _] : queue_to_passes_on_it)
	{
		for (auto& [_, sem] : p_queue->buf_to_sem)
		{
			all_waiting_queue_sems.push_back(sem);
		}
	}

	Queue* p_queue_used_to_finalize = nullptr;
	for (auto& [p_queue, _] : queue_to_passes_on_it)
	{
		//TODO:这里找到queue cap最强的queue，然后用来finalize。暂时先随便选一个queue
		if (!p_queue_used_to_finalize)
		{
			p_queue_used_to_finalize = p_queue;
		}
	}

	p_queue_used_to_finalize->FinalizeGeneralRendering(all_waiting_queue_sems, cur_frame, frame_number_semaphore);
}

void Anni::RenderGraphV1::DependencyGraph::CmdBufExecuteOnQueue(GraphicsPassNode* p_pass, uint32_t cur_frame,
	vk::CommandBuffer cmd_buf, Queue* execution_queue,
	VkSemaphore frame_num_sem,
	std::vector<vk::SemaphoreSubmitInfo>&
	wait_sem_submit_infos,
	std::vector<vk::SemaphoreSubmitInfo>&
	signal_sem_submit_infos)
{
	//wait and signal semaphore
	const uint64_t wait_value = cur_frame < (Vk::MAX_FRAMES_OVERLAP - 1) ? 0 : cur_frame - (Vk::MAX_FRAMES_OVERLAP - 1);
	wait_sem_submit_infos.emplace_back(
		frame_num_sem,
		wait_value,
		vk::PipelineStageFlagBits2::eAllCommands
	);

	//uint64_t signal_value = cur_frame;
	//signal_sem_submit_info.push_back(
	//	VkSemaphoreSubmitInfo{
	//		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
	//		.pNext = VK_NULL_HANDLE,
	//		.semaphore = frame_num_sem,
	//		.value = signal_value,
	//		.stageMask = 0,
	//		.deviceIndex = 0 });

	vk::CommandBufferSubmitInfo cmd_submit_info(cmd_buf);

	vk::SubmitInfo2 submit_info{};
	submit_info.setWaitSemaphoreInfos(wait_sem_submit_infos);
	submit_info.setCommandBufferInfos(cmd_submit_info);
	submit_info.setSignalSemaphoreInfos(signal_sem_submit_infos);

	execution_queue->GetQueue().submit2(submit_info);
}

void Anni::RenderGraphV1::DependencyGraph::GeneratePassNodeDepen()
{
	//DEPENDENCY GENERATION
	for (const auto& pass_node : pass_nodes)
	{
		//对于每一个inlet，会对其中的资源进行读或者写
		//case 0:如果是对这个资源进行了读取，则要去链上寻找前一个写入该资源的pass
		//case 1:如果是对这个资源进行了写入，则要去链上寻找前一个读取该资源的pass（因为会写入会暴力覆盖之前pass中写入的内容）
		//TODO: 多线程优化
		GeneratePassNodeDepen(*pass_node);

		// Remove all nullptr elements from the set

		pass_node->passes_depen_set.erase(nullptr);
	}


	//生成每个pass之后才能执行的pass 和 每个pass之前必须先执行的pass
	for (const auto& pass_node : pass_nodes)
	{
		for (const auto& p_pass_before_cur_pass : pass_node->passes_depen_set)
		{
			p_pass_before_cur_pass->passes_depen_on_cur_pass_set.insert(pass_node.get());
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::GeneratePassNodeDepen(GraphicsPassNode& pass_node)
{
	DependencyProducedFromBufInlet(pass_node);
	DependencyProducedFromBufOutlet(pass_node);
	DependencyProducedFromTexInlet(pass_node);
	DependencyProducedFromTexOutlet(pass_node);
}

void Anni::RenderGraphV1::DependencyGraph::DependencyProducedFromBufInlet(GraphicsPassNode& pass_node)
{
	for (auto& inlet : pass_node.ins_buf)
	{
		//在当前pass中的使用方式usage
		const BufUsage& buf_usage = inlet.GetRsrcUsage();


		//如果使用了读的方式，则
		if (RsrcAccessTypeRG::Read == buf_usage.GetRsrcAccessType())
		{
			GraphicsPassNode* previous_pass = &pass_node;
			auto& optinoal_ptr_providing_outlet = inlet.GetProvidingOutlet();
			while (true)
			{
				//一直往前找，直到找不到提供的接口
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					const auto providing_pass_access_type = providing_outlet.GetRsrcUsage().GetRsrcAccessType();
					if (providing_pass_access_type == RsrcAccessTypeRG::Write)
					{
						pass_node.passes_depen_set.insert(providing_pass);
						break;
					}
					if (providing_pass_access_type == RsrcAccessTypeRG::Read)
					{
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
				{
					//established资源在对应的创建pass中一定会被标记为写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set，所以下面的代码是多余的
					//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
					//{
					//	gfx_node->passes_depen_set.insert(previous_pass);
					//}
					break;
				}
			}
		}
		else if (RsrcAccessTypeRG::Write == buf_usage.GetRsrcAccessType())
		{
			GraphicsPassNode* previous_pass = &pass_node;
			auto& optinoal_ptr_providing_outlet = inlet.GetProvidingOutlet();
			while (true)
			{
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					const auto providing_pass_access_type = providing_outlet.GetRsrcUsage().GetRsrcAccessType();
					if (providing_pass_access_type == RsrcAccessTypeRG::Read)
					{
						pass_node.passes_depen_set.insert(providing_pass);
						break;
					}
					if (providing_pass_access_type == RsrcAccessTypeRG::Write)
					{
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
				{
					const auto& vsrc = inlet.GetPassAttachedTo()->GetVRsrcFromRsrcHandle(
						inlet.GetUnderlyingRsrcHandle());
					if (VRsrcType::Established == vsrc.rsrc_type)
					{
						//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
						if (previous_pass != &pass_node)
						{
							pass_node.passes_depen_set.insert(previous_pass);
						}
					}
					break;
				}
			}
		}
	}
}


void Anni::RenderGraphV1::DependencyGraph::DependencyProducedFromBufOutlet(GraphicsPassNode& pass_node)
{
	for (auto& outlet : pass_node.outs_buf)
	{
		//在当前pass中的使用方式usage
		const BufUsage& buf_usage = outlet.GetRsrcUsage();

		//如果使用了读的方式，则
		if (buf_usage.GetRsrcAccessType() == RsrcAccessTypeRG::Read)
		{
			GraphicsPassNode* previous_pass = &pass_node;
			auto& optinoal_ptr_providing_outlet = outlet.GetProvidingOutlet();

			while (true)
			{
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					const auto providing_pass_access_type = providing_outlet.GetRsrcUsage().GetRsrcAccessType();

					if (providing_pass_access_type == RsrcAccessTypeRG::Write)
					{
						pass_node.passes_depen_set.insert(providing_pass);
						break;
					}
					if (providing_pass_access_type == RsrcAccessTypeRG::Read)
					{
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
				{
					//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
					//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
					//{
					//	gfx_node->passes_depen_set.insert(previous_pass);
					//}

					break;
				}
			}
		}
		else if (buf_usage.GetRsrcAccessType() == RsrcAccessTypeRG::Write)
		{
			GraphicsPassNode* previous_pass = &pass_node;
			auto& optinoal_ptr_providing_outlet = outlet.GetProvidingOutlet();
			while (true)
			{
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					const auto providing_pass_access_type = providing_outlet.GetRsrcUsage().GetRsrcAccessType();
					if (providing_pass_access_type == RsrcAccessTypeRG::Read)
					{
						pass_node.passes_depen_set.insert(providing_pass);
						break;
					}
					if (providing_pass_access_type == RsrcAccessTypeRG::Write)
					{
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
				{
					const auto& vsrc = outlet.GetPassAttachedTo()->GetVRsrcFromRsrcHandle(
						outlet.GetUnderlyingRsrcHandle());

					if (VRsrcType::Established == vsrc.rsrc_type)
					{
						//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
						if (previous_pass != &pass_node)
						{
							pass_node.passes_depen_set.insert(previous_pass);
						}
					}
					break;
				}
			}
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::DependencyProducedFromTexInlet(GraphicsPassNode& pass_node)
{
	for (auto& inlet : pass_node.ins_tex)
	{
		//在当前pass中的使用方式usage
		std::variant<TexUsage, AttachUsage>& curlet_usage = inlet.GetRsrcUsage();
		const RsrcAccessTypeRG asscess_type
			= std::visit([](const auto& variant_usage) -> RsrcAccessTypeRG
				{
					return variant_usage.GetRsrcAccessType();
				}, curlet_usage);


		//如果使用了读的方式，则
		if (asscess_type == RsrcAccessTypeRG::Read)
		{
			ImgSyncInfo& previous_pass_syn_info = std::visit([](auto& variant_usage) -> ImgSyncInfo&
				{
					return variant_usage.GetSynInfo();
				}, curlet_usage);

			GraphicsPassNode* previous_pass = &pass_node;
			auto& optinoal_ptr_providing_outlet = inlet.GetProvidingOutlet();

			while (true)
			{
				//一直往前找，直到找不到提供的接口
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					auto& providing_pass_usage = providing_outlet.GetRsrcUsage();

					const ImgSyncInfo& providing_syn_info =
						std::visit([](auto& variant_usage) -> ImgSyncInfo&
							{
								return variant_usage.GetSynInfo();
							}, providing_pass_usage);

					const RsrcAccessTypeRG& providing_access_type =
						std::visit([](auto& variant_usage) -> RsrcAccessTypeRG
							{
								return variant_usage.GetRsrcAccessType();
							}, providing_pass_usage);


					if (providing_access_type == RsrcAccessTypeRG::Write)
					{
						pass_node.passes_depen_set.insert(providing_pass);
						break;
					}
					if (providing_access_type == RsrcAccessTypeRG::Read)
					{
						//先用之前的syn info做对比，如果img layout不同，那么就要当作写入看待。
						if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
						{
							pass_node.passes_depen_set.insert(providing_pass);
							break;
						}
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
					previous_pass_syn_info = providing_syn_info;
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
				{
					//established资源在对应的创建pass中一定会被标记为写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set，所以下面的代码是多余的
					//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
					//{
					//	gfx_node->passes_depen_set.insert(previous_pass);
					//}
					break;
				}
			}
		}
		else if (asscess_type == RsrcAccessTypeRG::Write)
		{
			ImgSyncInfo& previous_pass_syn_info = std::visit([](auto& variant_usage) -> ImgSyncInfo&
				{
					return variant_usage.GetSynInfo();
				}, curlet_usage);
			GraphicsPassNode* previous_pass = &pass_node;
			auto& optinoal_ptr_providing_outlet = inlet.GetProvidingOutlet();


			while (true)
			{
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					auto& providing_pass_usage = providing_outlet.GetRsrcUsage();
					const ImgSyncInfo& providing_syn_info =
						std::visit([](auto& variant_usage) -> ImgSyncInfo&
							{
								return variant_usage.GetSynInfo();
							}, providing_pass_usage);

					const RsrcAccessTypeRG& providing_access_type =
						std::visit([](auto& variant_usage) -> RsrcAccessTypeRG
							{
								return variant_usage.GetRsrcAccessType();
							}, providing_pass_usage);

					if (providing_access_type == RsrcAccessTypeRG::Read)
					{
						//先用之前的syninfo做对比，如果imglayout不同，那么就要当作写入看待。
						if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
						{
							optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
						}
						else
						{
							pass_node.passes_depen_set.insert(providing_pass);
							break;
						}
					}
					if (providing_access_type == RsrcAccessTypeRG::Write)
					{
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}

					previous_pass_syn_info = providing_syn_info;
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
				{
					const auto& vsrc = inlet.GetPassAttachedTo()->GetVRsrcFromRsrcHandle(
						inlet.GetUnderlyingRsrcHandle());
					if (VRsrcType::Established == vsrc.rsrc_type)
					{
						//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
						if (previous_pass != &pass_node)
						{
							pass_node.passes_depen_set.insert(previous_pass);
						}
					}
					break;
				}
			}
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::DependencyProducedFromTexOutlet(GraphicsPassNode& pass_node)
{
	for (auto& outlet : pass_node.outs_tex)
	{
		//在当前pass中的使用方式usage
		std::variant<TexUsage, AttachUsage>& curlet_usage = outlet.GetRsrcUsage();
		const RsrcAccessTypeRG access_type
			= std::visit([&](const auto& variant_usage) -> RsrcAccessTypeRG
				{
					return variant_usage.GetRsrcAccessType();
				}, curlet_usage);


		//如果使用了读的方式，则
		if (RsrcAccessTypeRG::Read == access_type)
		{
			GraphicsPassNode* previous_pass = &pass_node;
			auto optinoal_ptr_providing_outlet = outlet.GetProvidingOutlet();
			ImgSyncInfo& previous_pass_syn_info =
				std::visit([](auto& variant_usage) -> ImgSyncInfo&
					{
						return variant_usage.GetSynInfo();
					}, curlet_usage);

			while (true)
			{
				//providing_outlet_itor一定是outlet
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					auto& providing_pass_usage = providing_outlet.GetRsrcUsage();
					const ImgSyncInfo& providing_syn_info =
						std::visit([](auto& variant_usage) -> ImgSyncInfo&
							{
								return variant_usage.GetSynInfo();
							}, providing_pass_usage);

					const RsrcAccessTypeRG& providing_access_type =
						std::visit([](auto& variant_usage) -> RsrcAccessTypeRG
							{
								return variant_usage.GetRsrcAccessType();
							}, providing_pass_usage);


					if (providing_access_type == RsrcAccessTypeRG::Write)
					{
						pass_node.passes_depen_set.insert(providing_pass);
						break;
					}
					if (providing_access_type == RsrcAccessTypeRG::Read)
					{
						//先用之前的syn info做对比，如果img layout不同，那么就要当作写入看待。
						if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
						{
							pass_node.passes_depen_set.insert(providing_pass);
							break;
						}
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
					previous_pass_syn_info = providing_syn_info;
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
				{
					//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
					//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
					//{
					//	gfx_node->passes_depen_set.insert(previous_pass);
					//}

					break;
				}
			}
		}
		else if (RsrcAccessTypeRG::Write == access_type)
		{
			GraphicsPassNode* previous_pass = &pass_node;
			auto optinoal_ptr_providing_outlet = outlet.GetProvidingOutlet();
			const ImgSyncInfo& previous_pass_syn_info =
				std::visit([](auto& variant_usage) -> ImgSyncInfo&
					{
						return variant_usage.GetSynInfo();
					}, curlet_usage);


			while (true)
			{
				if (optinoal_ptr_providing_outlet)
				{
					auto providing_pass = optinoal_ptr_providing_outlet.value().pass_attached_to;
					auto& providing_outlet = providing_pass->GetOulet(optinoal_ptr_providing_outlet.value());
					previous_pass = providing_pass;

					auto& providing_pass_usage = providing_outlet.GetRsrcUsage();
					const ImgSyncInfo& providing_syn_info =
						std::visit([](auto& variant_usage) -> ImgSyncInfo&
							{
								return variant_usage.GetSynInfo();
							}, providing_pass_usage);

					const RsrcAccessTypeRG& providing_access_type =
						std::visit([](auto& variant_usage) -> RsrcAccessTypeRG
							{
								return variant_usage.GetRsrcAccessType();
							}, providing_pass_usage);


					if (providing_access_type == RsrcAccessTypeRG::Read)
					{
						//先用之前的syn info做对比，如果img layout不同，那么就要当作写入看待。
						if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
						{
							optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
						}
						else
						{
							pass_node.passes_depen_set.insert(providing_pass);
							break;
						}
					}
					if (providing_access_type == RsrcAccessTypeRG::Write)
					{
						optinoal_ptr_providing_outlet = providing_outlet.GetProvidingOutlet();
					}
				}
				else //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
				{
					const auto& vsrc = outlet.GetPassAttachedTo()->GetVRsrcFromRsrcHandle(
						outlet.GetUnderlyingRsrcHandle());
					if (VRsrcType::Established == vsrc.rsrc_type)
					{
						//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
						if (previous_pass != &pass_node)
						{
							pass_node.passes_depen_set.insert(previous_pass);
						}
					}
					break;
				}
			}
		}
	}
}


void Anni::RenderGraphV1::DependencyGraph::TopologicalDFSSort()
{
	//拓扑排序结合dfs算法，划分层次
	std::vector<bool> visitedNodes(pass_nodes.size(), false);
	std::vector<bool> onStackNodes(pass_nodes.size(), false);

	for (auto gfx_node_itr = pass_nodes.begin(); gfx_node_itr < pass_nodes.end(); ++gfx_node_itr)
	{
		const auto index = std::distance(pass_nodes.begin(), gfx_node_itr);

		VisitNode(index, gfx_node_itr->get(), visitedNodes, onStackNodes, topologically_sorted_nodes);

		if (topologically_sorted_nodes.size() == pass_nodes.size())
		{
			break;
		}
	}
	std::ranges::reverse(topologically_sorted_nodes);
}


void Anni::RenderGraphV1::DependencyGraph::VisitNode(size_t cur_index, GraphicsPassNode* cur_node_pass,
	std::vector<bool>& visited, std::vector<bool>& on_stack,
	std::vector<GraphicsPassNode*>& topologically_sorted_nodes)
{
	if (visited[cur_index] && on_stack[cur_index])
	{
		ASSERT_WITH_MSG(false, "found a circular dependency and must abort.");
	}
	if (visited[cur_index])
	{
		return;
	}

	visited[cur_index] = true;
	on_stack[cur_index] = true;

	for (GraphicsPassNode* adj_list : cur_node_pass->passes_depen_on_cur_pass_set)
	{
		VisitNode(FindIndexInPassNodeArray(adj_list, pass_nodes), adj_list, visited, on_stack,
			topologically_sorted_nodes);
	}

	topologically_sorted_nodes.push_back(cur_node_pass);
	on_stack[cur_index] = false;
}


size_t Anni::RenderGraphV1::DependencyGraph::FindIndexInPassNodeArray(
	const GraphicsPassNode* const  node_pass,
	const std::vector<std::unique_ptr<GraphicsPassNode>>& all_nodes
)
{
	for (auto gfx_node_itr = all_nodes.begin(); gfx_node_itr < all_nodes.end(); ++gfx_node_itr)
	{
		if (gfx_node_itr->get() == node_pass)
		{
			return std::distance(all_nodes.begin(), gfx_node_itr);
		}
	}
	assert(!"no pass found!");
	return 0;
}

void Anni::RenderGraphV1::DependencyGraph::DependencyLevelsGeneration()
{
	///************************************************************
	//DEPENDENCY LEVELS(LAYERS) GENERATION层次划分:
	///************************************************************
	std::vector<size_t> cur_level(topologically_sorted_nodes.size(), 0);
	for (auto pas_itr = topologically_sorted_nodes.begin(); pas_itr < topologically_sorted_nodes.end(); ++pas_itr)
	{
		GraphicsPassNode* p_cur_pass = *pas_itr;

		size_t cur_index = std::distance(topologically_sorted_nodes.begin(), pas_itr);
		for (auto adjacent_pass : p_cur_pass->passes_depen_on_cur_pass_set)
		{
			// Use std::find_if with a lambda function
			auto it_adj_pass = std::ranges::find_if(topologically_sorted_nodes,
				[adjacent_pass](const GraphicsPassNode* node_itr)
				{
					return node_itr == adjacent_pass;
				});
			assert(it_adj_pass != topologically_sorted_nodes.end());

			size_t adj_index = std::distance(topologically_sorted_nodes.begin(), it_adj_pass);
			if (cur_level[adj_index] < cur_level[cur_index] + 1)
			{
				cur_level[adj_index] = cur_level[cur_index] + 1;
			}
		}
	}

	for (int i = 0; i < topologically_sorted_nodes.size(); ++i)
	{
		topologically_sorted_nodes[i]->cur_level = cur_level[i];
	}
	const auto max_level_itr = std::ranges::max_element(cur_level);
	max_level = *max_level_itr;
}

void Anni::RenderGraphV1::DependencyGraph::GPUWorkDistribution()
{
	///************************************************************
	//DISTRIBUTE WORK TO MULTIPLE QUEUES ONTO GPU:
	///************************************************************
	//std::unordered_map<DeviceManager::VkExecutionQueue *, std::list<PassNode *>>         queue_to_passes_on_it;
	//std::unordered_map<PassNode *, std::pair<DeviceManager::VkExecutionQueue *, size_t>> pass_to_queue_info;

	queue_manager.QueuesNumDisbatchedPassesToZero();

	size_t monotonical_assigned_index = 1;
	for (size_t level = 0; level <= max_level; ++level)
	{
		for (auto& ptr_pass : topologically_sorted_nodes)
		{
			if (ptr_pass->cur_level == level)
			{
				auto& target_queue = GetSuitableQueue(*ptr_pass);
				auto ptr_queue = &target_queue;


				if (queue_to_passes_on_it.contains(ptr_queue))
				{
					queue_to_passes_on_it[ptr_queue].push_back(ptr_pass);
				}
				else
				{
					queue_to_passes_on_it.emplace(ptr_queue, std::vector<GraphicsPassNode*>{ptr_pass});
				}

				auto result_itr = pass_to_queue_info.emplace(ptr_pass, std::pair<Queue*, size_t>{
					ptr_queue, monotonical_assigned_index
				});

				ASSERT_WITH_MSG(result_itr.second, "the pass has been distributed to queue!");
				monotonical_assigned_index++;
			}
		}
	}

	std::vector<Queue*> all_queue_in_use; //随意确定一个queue的顺序，方便之后用index索引
	// Extract keys from the map and store them in the vector
	std::ranges::transform(queue_to_passes_on_it, std::back_inserter(all_queue_in_use),
		[](const std::pair<Queue*, std::vector<GraphicsPassNode*>>& pair)
		{
			return pair.first; // Extract the key
		});
}

Anni::Queue& Anni::RenderGraphV1::DependencyGraph::GetSuitableQueue(GraphicsPassNode& pass)
{
	const PassType pass_type = pass.GetRenderpassType();
	QueueCapability queue_cap_requirement;

	if (pass_type & Graphics)
	{
		queue_cap_requirement.graphics = true;
	}

	if (pass_type & Compute)
	{
		queue_cap_requirement.compute = true;
	}

	if (pass_type & Transfor)
	{
		queue_cap_requirement.transfer = true;
	}

	if (pass_type & Present)
	{
		queue_cap_requirement.present = true;
	}
	auto& target_queue = queue_manager.GetSuitableQueue(queue_cap_requirement);
	return target_queue;
}

void Anni::RenderGraphV1::DependencyGraph::SyncPrimitivesAquisition()
{
	for (auto itr = topologically_sorted_nodes.begin(); std::next(itr) != topologically_sorted_nodes.end(); ++itr)
	{
		diff_queue_sync_sema.emplace(PassNodePair{ *itr, *std::next(itr) }, semaphore_pool.AcquireTimelineSem());
	}
}

void Anni::RenderGraphV1::DependencyGraph::MarkLayersMultiAccessSameRsrc()
{
	for (size_t i = 0; i <= max_level; ++i)
	{
		if_level_accesses_same_rsrc_multimes.insert_or_assign(i, false);
	}

	for (auto& vrsrc : rg_buffers)
	{
		for (auto& [level, passes_at_the_same_level] : vrsrc.level_to_passes_attached_to)
		{
			if (passes_at_the_same_level.size() > 1)
			{
				if_level_accesses_same_rsrc_multimes[level] = true;
			}
		}
	}

	for (auto& vrsrc : rg_textures)
	{
		for (auto& [level, passes_at_the_same_level] : vrsrc.level_to_passes_attached_to)
		{
			if (passes_at_the_same_level.size() > 1)
			{
				if_level_accesses_same_rsrc_multimes[level] = true;
			}
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::PartitionPassesAtTheSameLevel()
{
	///************************************************************
	//同level的所有passes排序:
	///************************************************************

	//首先对于所有的资源，所有使用到资源的pass按照layer排序以后，每一个layer层级内的pass都需要进行排序，否则这个执行顺序定不下来，根本写不了同步
	for (auto& vrsrc : rg_buffers)
	{
		for (size_t level = 0; level <= max_level; ++level)
		{
			vrsrc.level_to_passes_attached_to.emplace(level, std::vector<GraphicsPassNode*>{});
			vrsrc.level_to_passes_attached_to_partitioned.emplace(level, std::vector<std::vector<GraphicsPassNode*>>{});
		}

		//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
		std::ranges::for_each(
			vrsrc.passes_access_this_rsrc,
			[&vrsrc](const std::pair<GraphicsPassNode*, RsrcAccessTypeRG>& pass_assess_pair)
			{
				auto cur_pass = pass_assess_pair.first;
				vrsrc.level_to_passes_attached_to.at(cur_pass->cur_level).push_back(cur_pass);
			});


		//必须把这些同layer的passes按照他们执行的queue fam进行分块
		//queue fam分块内还要进行 queue 的分块
		for (auto& [level, passes_at_same_levels] : vrsrc.level_to_passes_attached_to)
		{
			//for (auto& pass : passes_at_same_levels)
			//{
			//	vrsrc.level_to_unique_queues.at(level).insert(pass_to_queue_info.at(pass).first);
			//}

			std::set<size_t> level_to_unique_queue_fam_index; //这里用set很关键。
			for (auto& pass : passes_at_same_levels)
			{
				level_to_unique_queue_fam_index.insert(
					pass_to_queue_info.at(pass).first->GetQueueCap().queue_family_index);
			}

			std::vector<std::vector<GraphicsPassNode*>> partitions_by_queue_fam_then_by_queue;

			std::vector<GraphicsPassNode*> partition_result;
			for (auto unique_queue_fam : level_to_unique_queue_fam_index) //unique queue为指针
			{
				//先用queue family做partition
				std::vector<GraphicsPassNode*> queue_fam_partition;
				std::ranges::copy_if(passes_at_same_levels, std::back_inserter(queue_fam_partition),
					[this, unique_queue_fam](GraphicsPassNode* const pass) -> bool
					{
						return (this->pass_to_queue_info.at(pass).first->GetQueueCap().
							queue_family_index == unique_queue_fam);
					});
				//上面执行后，queue_fam_partition中所有的pass都是属于同一个queue family的

				std::set<Queue*> unique_queues_on_the_same_queue_fam; //还是要用std::set
				std::ranges::for_each
				(
					queue_fam_partition,
					[this, &unique_queues_on_the_same_queue_fam](GraphicsPassNode* pass)
					{
						unique_queues_on_the_same_queue_fam.insert(this->pass_to_queue_info.at(pass).first);
					}
				);
				//继续用queue来做partition：
				for (auto unique_queue : unique_queues_on_the_same_queue_fam) //unique queue为指针
				{
					std::set<GraphicsPassNode*> partition;
					std::ranges::copy_if(queue_fam_partition, std::inserter(partition, partition.begin()),
						[this, unique_queue](GraphicsPassNode* pass) -> bool
						{
							return this->pass_to_queue_info.at(pass).first == unique_queue;
						});
					partition_result.insert(partition_result.end(), partition.begin(), partition.end());
					partitions_by_queue_fam_then_by_queue.push_back(std::vector<GraphicsPassNode*>{
						partition.begin(), partition.end()
					});
				}
			}
			//pass0 Queue0 | pass1 Queue0 | pass2 Queue0 | pass3 Queue1 | pass4 Queue1 | pass5 Queue1 这里保证了不同queue不会混在一起
			passes_at_same_levels = std::move(partition_result);
			vrsrc.level_to_passes_attached_to_partitioned.insert_or_assign(
				level, std::move(partitions_by_queue_fam_then_by_queue));
		}
	}


	//首先对于所有的资源，所有使用到资源的pass按照layer排序以后，每一个layer层级内的pass都需要进行排序，否则这个执行顺序定不下来，根本写不了同步
	for (auto& vrsrc : rg_textures)
	{
		for (auto level = 0; level <= max_level; ++level)
		{
			vrsrc.level_to_passes_attached_to.emplace(level, std::vector<GraphicsPassNode*>{});
			vrsrc.level_to_passes_attached_to_partitioned.emplace(level, std::vector<std::vector<GraphicsPassNode*>>{});
			//vrsrc.level_to_t_semas.emplace(level,std::make_shared<TimelineSemWrapper>( device_manager) );
		}

		//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
		std::ranges::for_each(
			vrsrc.passes_access_this_rsrc,
			[&vrsrc](const std::pair<GraphicsPassNode*, RsrcAccessTypeRG>& pass_assess_pair)
			{
				auto cur_pass = pass_assess_pair.first;
				vrsrc.level_to_passes_attached_to.at(cur_pass->cur_level).push_back(cur_pass);
			});


		//必须把这些同layer的passes按照他们执行的queue fam进行分块
		//queue fam分块内还要进行 queue 的分块
		for (auto& [level, passes_at_same_levels] : vrsrc.level_to_passes_attached_to)
		{
			//for (auto& pass : passes_at_same_levels)
			//{
			//	vrsrc.level_to_unique_queues.at(level).insert(pass_to_queue_info.at(pass).first);
			//}

			std::set<size_t> level_to_unique_queue_fam_index;
			for (auto& pass : passes_at_same_levels)
			{
				level_to_unique_queue_fam_index.insert(
					pass_to_queue_info.at(pass).first->GetQueueCap().queue_family_index);
			}

			std::vector<std::vector<GraphicsPassNode*>> partitions_by_queue_fam_then_by_queue;
			std::vector<GraphicsPassNode*> partition_result;
			for (auto unique_queue_fam : level_to_unique_queue_fam_index) //unique queue为指针
			{
				//先用queue family做partition
				std::vector<GraphicsPassNode*> queue_fam_partition;
				std::ranges::copy_if(passes_at_same_levels, std::back_inserter(queue_fam_partition),
					[this, unique_queue_fam](GraphicsPassNode* const pass) -> bool
					{
						return (this->pass_to_queue_info.at(pass).first->GetQueueCap().
							queue_family_index == unique_queue_fam);
					});
				//上面执行后，queue_fam_partition中所有的pass都是属于同一个queue family的

				std::unordered_set<Queue*> unique_queues_on_the_same_queue_fam;
				std::ranges::for_each
				(
					queue_fam_partition,
					[this, &unique_queues_on_the_same_queue_fam](GraphicsPassNode* pass)
					{
						unique_queues_on_the_same_queue_fam.insert(this->pass_to_queue_info.at(pass).first);
					}
				);
				//继续用queue来做partition：
				for (auto unique_queue : unique_queues_on_the_same_queue_fam) //unique queue为指针
				{
					std::set<GraphicsPassNode*> partition;
					std::ranges::copy_if(queue_fam_partition, std::inserter(partition, partition.begin()),

						[this, unique_queue](GraphicsPassNode* pass) -> bool
						{
							return this->pass_to_queue_info.at(pass).first == unique_queue;
						});

					partition_result.insert(partition_result.end(), partition.begin(), partition.end());
					partitions_by_queue_fam_then_by_queue.push_back(std::vector<GraphicsPassNode*>{
						partition.begin(), partition.end()
					});
				}
			}
			//pass0 Queue0 | pass1 Queue0 | pass2 Queue0 | pass3 Queue1 | pass4 Queue1 | pass5 Queue1 这里保证了不同queue不会混在一起
			passes_at_same_levels = std::move(partition_result);
			vrsrc.level_to_passes_attached_to_partitioned.insert_or_assign(
				level, std::move(partitions_by_queue_fam_then_by_queue));
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::SortPassesAccessingRsrc()
{
	//对每个资源的使用者（passes）进行排序
	for (auto& vrsc : rg_buffers)
	{
		// Sorting the unordered vector based on the order of the order vector(that is, topologically_sorted_nodes)
		std::ranges::sort(
			vrsc.passes_access_this_rsrc,
			[&](const std::pair<GraphicsPassNode*, RsrcAccessTypeRG>& pair_left,
				const std::pair<GraphicsPassNode*, RsrcAccessTypeRG>& pair_right)
			{
				return SortByTopologicallySortedPasses(pair_left.first, pair_right.first, topologically_sorted_nodes);
			});
	}

	for (auto& vrsc : rg_textures)
	{
		// Sorting the unordered vector based on the order of the order vector
		std::ranges::sort
		(
			vrsc.passes_access_this_rsrc,
			[&](const std::pair<GraphicsPassNode*, RsrcAccessTypeRG>& pair_left,
				const std::pair<GraphicsPassNode*, RsrcAccessTypeRG>& pair_right)
			{
				return SortByTopologicallySortedPasses(pair_left.first, pair_right.first, topologically_sorted_nodes);
			}
		);
	}
}

bool Anni::RenderGraphV1::DependencyGraph::SortByTopologicallySortedPasses(
	const GraphicsPassNode* a, const GraphicsPassNode* b, const std::vector<GraphicsPassNode*>& order)
{
	const auto it_a = std::ranges::find(order, a);
	const auto it_b = std::ranges::find(order, b);

	// If both elements are found in the order vector, compare their positions
	if (it_a != order.end() && it_b != order.end())
	{
		return std::distance(order.begin(), it_a) < std::distance(order.begin(), it_b);
	}

	// If only one element is found, prioritize it
	if (it_a != order.end())
	{
		return true;
	}
	if (it_b != order.end())
	{
		return false;
	}
	// If neither element is found, maintain the original order
	return a < b;
}


void Anni::RenderGraphV1::DependencyGraph::SyncPrimitivesInsertion()
{
	vec_swap_imgs_finshed_using.emplace_back(std::vector<std::shared_ptr<BinarySemWrapper>>{}, cur_frame);
	///************************************************************
	//SYNC PRIMITIVES INSERTION同步原语插入:
	///************************************************************
	//按照level层次来loop，
	for (size_t cur_level = 0; cur_level <= max_level; ++cur_level)
	{
		for (GraphicsPassNode* cur_pass : topologically_sorted_nodes)
		{
			if (cur_pass->cur_level == cur_level)
			{
				SyncPrimitivesInsertionInletBuffer(cur_pass);
				SyncPrimitivesInsertionOutletBuffer(cur_pass);

				SyncPrimitivesInsertionInletTexture(cur_pass);
				SyncPrimitivesInsertionOutletTexture(cur_pass);
			}
		}
	}


	//value up for timeline sync sema
	for (const auto& [_, p_sem] : diff_queue_sync_sema)
	{
		++p_sem->GetLastValue();
	}
}

void Anni::RenderGraphV1::DependencyGraph::SyncPrimitivesInsertionInletBuffer(GraphicsPassNode* const cur_pass)
{
	auto cur_pass_level = cur_pass->cur_level;
	for (auto& cur_let : cur_pass->ins_buf)
	{
		VirtualBuffer& curlet_underlying_rsrc = cur_pass->GetVRsrcFromRsrcHandle(cur_let.GetUnderlyingRsrcHandle());
		BufUsage& curlet_usage = cur_let.GetRsrcUsage();
		const IRsrcUsage::RsrcOrigin curlet_origin = curlet_usage.GetRsrcOrigin();

		//直接在当前pass建立的资源，同步原语插入：
		if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
		{
			auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
			cur_pass->InsertSyncInfoForInitalUsage(
				source_sync,
				curlet_usage.GetSynInfo(),
				cur_let.GetUnderlyingRsrcHandle()
			);
		}

		if (IRsrcUsage::RsrcOrigin::FromOutSide == curlet_origin)
		{
			auto first_pass = curlet_underlying_rsrc.passes_access_this_rsrc.front().first;
			if (first_pass == cur_pass)
			{
				//TODO: if some shit is loaded into the buffer before the resource is imported into rendergraph, then host to device sync needs to be inserted here. But for now, we just use SynInfoOnLoad;
				const auto& source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
				cur_pass->InsertSyncInfoForInitalUsage(
					source_sync,
					curlet_usage.GetSynInfo(),
					cur_let.GetUnderlyingRsrcHandle()
				);
			}
		}


		//拿到所有使用该资源的pass，对于之前的每一个layer，进行同步原语插入。比如当前layer是5，我们首先需要找到layer=0,1,2,3,4
		for (const auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.
			level_to_passes_attached_to_partitioned)
		{
			//如果前面的layer i 出现了对某一资源同时access的情况，
			//layer i 可能含有多个执行在不同queue上的passes，不过他们是按照分块排序的，这种情况下，我们只需要在最后一块中的所有pass和当前pass之间插入同步即可
			if (level < cur_pass_level)
			{
				const auto& last_passes_group = passes_at_same_level_partitioned.back();
				const auto last_queue = pass_to_queue_info.at(last_passes_group.back());
				const auto queue_which_cur_pass_execute = pass_to_queue_info.at(cur_pass);

				if (last_queue == queue_which_cur_pass_execute)
				{
					auto last_pass_executing_on_last_queue_group = last_passes_group.back();
					auto& target_let = cur_let;

					auto sor_pass = last_pass_executing_on_last_queue_group;
					auto tar_pass = cur_pass;
					auto underlying_buf_handle = target_let.GetUnderlyingRsrcHandle();


					auto founded_buf_outlet_usage = std::ranges::find_if(sor_pass->outs_buf,
						[&underlying_buf_handle](
							const RsrcOutlet<BufUsage>& outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});

					auto founded_buf_inlet_usage = std::ranges::find_if(sor_pass->ins_buf,
						[&underlying_buf_handle](
							const RsrcInlet<BufUsage>& inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});
					BufUsage* p_sor_buf_usage = nullptr;
					if (founded_buf_outlet_usage != sor_pass->outs_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_outlet_usage->usage);
					}
					if (founded_buf_inlet_usage != sor_pass->ins_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_inlet_usage->usage);
					}


					BufUsage& sor_buf_usage = *p_sor_buf_usage;
					BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

					cur_pass->InsertSameQueueSyncInfo(
						sor_pass,
						tar_pass,
						queue_which_cur_pass_execute.first,
						sor_buf_usage.GetSynInfo(),
						tar_buf_usage.GetSynInfo(),
						underlying_buf_handle
					);
				}
				else
				{
					const auto tar_pass = cur_pass;
					const auto sor_pass = last_passes_group.back();
					const auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					const auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

					auto& target_let = cur_let;
					const VirtualBuffer::Handle underlying_buf_handle = target_let.GetUnderlyingRsrcHandle();

					auto founded_buf_outlet_usage = std::ranges::find_if(sor_pass->outs_buf,
						[&underlying_buf_handle](const RsrcOutlet<BufUsage>& outlet_buf)
						{
							return outlet_buf.rsrc_handle == underlying_buf_handle;
						});

					auto founded_buf_inlet_usage = std::ranges::find_if(sor_pass->ins_buf,
						[&underlying_buf_handle](
							const RsrcInlet<BufUsage>& inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});
					BufUsage* p_sor_buf_usage = nullptr;
					if (founded_buf_outlet_usage != sor_pass->outs_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_outlet_usage->usage);
					}
					if (founded_buf_inlet_usage != sor_pass->ins_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_inlet_usage->usage);
					}

					BufUsage& sor_buf_usage = *p_sor_buf_usage;
					BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

					const SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
					const WaitValue tar_wait_val{ sync_sema->GetLastValue() + 1 };

					SemInsertInfoSafe sor_insert_info(
						sor_signal_val,
						sor_buf_usage.sync_info.stage_mask
					);

					SemInsertInfoSafe tar_insert_info{
						tar_wait_val,
						tar_buf_usage.sync_info.stage_mask
					};

					sor_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_buf_usage.GetSynInfo(),
						tar_buf_usage.GetSynInfo(),
						target_let.GetUnderlyingRsrcHandle(),
						sor_insert_info,
						tar_insert_info
					);

					tar_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_buf_usage.GetSynInfo(),
						tar_buf_usage.GetSynInfo(),
						target_let.GetUnderlyingRsrcHandle(),
						sor_insert_info,
						tar_insert_info
					);
				}
			}


			//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
			if (level == cur_pass_level)
			{
				//passes_at_same_levels排布一定是 按照queue 分块的
				// 比如passes_at_same_levels是   pass0 , pass1 , pass4, pass5 , pass7 , pass9
				// 块0  queue3 ：|pass0，pass1，pass4| ;  块1  queue 2 ：|pass5，pass7，pass9|

				//在同一个level中决定如何同步：在同一level中的pass应该只会出现同时读一个资源的情况，这种情况只需要处理不同queue的queue ownership transfer的问题?
				//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);

				auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];

				std::vector<Queue*> unique_queues_ordered;
				std::ranges::transform(passes_at_same_level_partitioned, std::back_inserter(unique_queues_ordered),
					[this](const std::vector<GraphicsPassNode*>& inner_pass_group)-> Queue*
					{
						return pass_to_queue_info.at(inner_pass_group.front()).first;
					}
				);


				//auto  cur_pass_pos_in_cur_level = std::ranges::find(passes_at_same_levels.begin(), passes_at_same_levels.end(), cur_pass);
				auto cur_pass_pos_in_cur_level = std::ranges::find(passes_at_same_levels, cur_pass);
				auto cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);

				ASSERT_WITH_MSG(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
				ASSERT_WITH_MSG(cur_pass_pos_in_cur_level != passes_at_same_levels.end(),
					"Current pass is not found at the level!");

				auto sor_pass = cur_pass;
				auto tar_pass = cur_pass;

				//如果当前pass不是这个level中第一个pass
				if (cur_pass_pos_in_cur_level != passes_at_same_levels.begin())
				{
					// Get the iterator to the previous element
					auto previous_pass_itr = std::prev(cur_pass_pos_in_cur_level);
					// Access the previous element
					sor_pass = *previous_pass_itr;
					tar_pass = cur_pass;

					const auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					const auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
					auto& target_let = cur_let;

					const auto underlying_buf_handle = cur_let.GetUnderlyingRsrcHandle();


					auto founded_buf_outlet_usage = std::ranges::find_if(sor_pass->outs_buf,
						[&underlying_buf_handle](
							const RsrcOutlet<BufUsage>& outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});

					auto founded_buf_inlet_usage = std::ranges::find_if(sor_pass->ins_buf,
						[&underlying_buf_handle](
							const RsrcInlet<BufUsage>& inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});
					BufUsage* p_sor_buf_usage = nullptr;
					if (founded_buf_outlet_usage != sor_pass->outs_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_outlet_usage->usage);
					}
					if (founded_buf_inlet_usage != sor_pass->ins_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_inlet_usage->usage);
					}


					BufUsage& sor_buf_usage = *p_sor_buf_usage;
					BufUsage& tar_buf_usage = target_let.GetRsrcUsage();


					//Source pass     ------------------------->       Cur pass
					//Sync primitive
					//auto base_val = sync_sema->GetLastValue();
					//// Element found in the set
					//std::size_t index = std::distance(unique_queues_ordered.begin(), cur_queue_itr);
					//uint64_t    sor_wait_value = base_val + index - 1;         //0
					//uint64_t    sor_signal_value = base_val + index;           //1

					//uint64_t    tar_wait_value = base_val + index;             //1
					//uint64_t    tar_signal_value = base_val + index + 1;       //2

					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

					const SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
					const WaitValue tar_wait_val{ sync_sema->GetLastValue() + 1 };

					SemInsertInfoSafe sor_insert_info(
						sor_signal_val,
						sor_buf_usage.sync_info.stage_mask
					);

					SemInsertInfoSafe tar_insert_info{
						tar_wait_val,
						tar_buf_usage.sync_info.stage_mask
					};


					//CASE 1: previous pass and current pass in a same queue 
					if (sor_pass_queue_ptr == tar_pass_queue_ptr)
					{
						//because we use barrier, so only one barrier is needed.
						tar_pass->InsertSameQueueSyncInfo(
							sor_pass,
							tar_pass,
							tar_pass_queue_ptr,
							sor_buf_usage.GetSynInfo(),
							tar_buf_usage.GetSynInfo(),
							target_let.GetUnderlyingRsrcHandle()
						);
					}
					//CASE 2: DIFF queue situation (need queue ownership transfer!)
					else
					{
						sor_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_buf_usage.GetSynInfo(),
							tar_buf_usage.GetSynInfo(),
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);

						tar_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_buf_usage.GetSynInfo(),
							tar_buf_usage.GetSynInfo(),
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);
					}
				}
			}
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::SyncPrimitivesInsertionOutletBuffer(GraphicsPassNode* const cur_pass)
{
	auto cur_level = cur_pass->cur_level;
	for (auto& cur_let : cur_pass->outs_buf)
	{
		VirtualBuffer& curlet_underlying_rsrc = cur_pass->GetVRsrcFromRsrcHandle(cur_let.GetUnderlyingRsrcHandle());
		BufUsage& curlet_usage = cur_let.GetRsrcUsage();
		const IRsrcUsage::RsrcOrigin curlet_origin = curlet_usage.GetRsrcOrigin();


		//直接在当前pass建立的资源，同步原语插入：
		if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
		{
			auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
			cur_pass->InsertSyncInfoForInitalUsage(
				source_sync,
				curlet_usage.GetSynInfo(),
				cur_let.GetUnderlyingRsrcHandle()
			);
		}

		if (IRsrcUsage::RsrcOrigin::FromOutSide == curlet_origin)
		{
			auto first_pass = curlet_underlying_rsrc.passes_access_this_rsrc.front().first;
			if (first_pass == cur_pass)
			{
				//TODO: if some shit is loaded into the buffer before imported into rendergraph, then host to device sync needs to be inserted here.
				const auto& source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
				cur_pass->InsertSyncInfoForInitalUsage(
					source_sync,
					curlet_usage.GetSynInfo(),
					cur_let.GetUnderlyingRsrcHandle());
			}
		}

		//拿到所有使用该资源的pass，对于之前的每一个layer，进行同步原语插入。比如当前layer是5，我们首先需要找到layer=0,1,2,3,4
		for (const auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.
			level_to_passes_attached_to_partitioned)
		{
			//如果前面的layer i 出现了对某一资源同时access的情况，
			//layer i 可能含有多个执行在不同queue上的passes，不过他们是按照分块排序的，这种情况下，我们只需要在最后一块中的所有pass和当前pass之间插入同步即可
			if (level < cur_level)
			{
				const auto& last_passes_group = passes_at_same_level_partitioned.back();
				const auto last_queue = pass_to_queue_info.at(last_passes_group.back());
				const auto queue_which_cur_pass_execute = pass_to_queue_info.at(cur_pass);

				if (last_queue == queue_which_cur_pass_execute)
				{
					auto last_pass_executing_on_last_queue_group = last_passes_group.back();
					auto& target_let = cur_let;
					auto underlying_buf_handle = target_let.GetUnderlyingRsrcHandle();

					auto sor_pass = last_pass_executing_on_last_queue_group;
					auto tar_pass = cur_pass;

					auto founded_buf_outlet_usage = std::ranges::find_if(sor_pass->outs_buf,
						[&underlying_buf_handle](
							const RsrcOutlet<BufUsage>& outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});

					auto founded_buf_inlet_usage = std::ranges::find_if(sor_pass->ins_buf,
						[&underlying_buf_handle](
							const RsrcInlet<BufUsage>& inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});
					BufUsage* p_sor_buf_usage = nullptr;
					if (founded_buf_outlet_usage != sor_pass->outs_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_outlet_usage->usage);
					}
					if (founded_buf_inlet_usage != sor_pass->ins_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_inlet_usage->usage);
					}


					BufUsage& sor_buf_usage = *p_sor_buf_usage;
					BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

					cur_pass->InsertSameQueueSyncInfo(
						sor_pass,
						tar_pass,
						queue_which_cur_pass_execute.first,
						sor_buf_usage.GetSynInfo(),
						tar_buf_usage.GetSynInfo(),
						underlying_buf_handle
					);
				}
				else
				{
					const auto tar_pass = cur_pass;
					const auto sor_pass = last_passes_group.back();
					const auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					const auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });
					auto& target_let = cur_let;
					const auto underlying_buf_handle = target_let.GetUnderlyingRsrcHandle();


					auto founded_buf_outlet_usage = std::ranges::find_if(sor_pass->outs_buf,
						[&underlying_buf_handle](
							const RsrcOutlet<BufUsage>& outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});

					auto founded_buf_inlet_usage = std::ranges::find_if(sor_pass->ins_buf,
						[&underlying_buf_handle](
							const RsrcInlet<BufUsage>& inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});
					BufUsage* p_sor_buf_usage = nullptr;
					if (founded_buf_outlet_usage != sor_pass->outs_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_outlet_usage->usage);
					}
					if (founded_buf_inlet_usage != sor_pass->ins_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_inlet_usage->usage);
					}


					BufUsage& sor_buf_usage = *p_sor_buf_usage;
					BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

					const SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
					const WaitValue tar_wait_val{ sync_sema->GetLastValue() + 1 };

					SemInsertInfoSafe sor_insert_info(
						sor_signal_val,
						sor_buf_usage.sync_info.stage_mask
					);

					SemInsertInfoSafe tar_insert_info{
						tar_wait_val,
						tar_buf_usage.sync_info.stage_mask
					};


					sor_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_buf_usage.GetSynInfo(),
						tar_buf_usage.GetSynInfo(),
						target_let.GetUnderlyingRsrcHandle(),
						sor_insert_info,
						tar_insert_info
					);

					tar_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_buf_usage.GetSynInfo(),
						tar_buf_usage.GetSynInfo(),
						target_let.GetUnderlyingRsrcHandle(),
						sor_insert_info,
						tar_insert_info
					);
				}
			}
			//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
			if (level == cur_level)
			{
				//passes_at_same_levels排布一定是 按照queue 分块的
				// 比如passes_at_same_levels是   pass0 , pass1 , pass4, pass5 , pass7 , pass9
				// 块0  queue3 ：|pass0，pass1，pass4| ;  块1  queue 2 ：|pass5，pass7，pass9|

				//在同一个level中决定如何同步：在同一level中的pass应该只会出现同时读一个资源的情况，这种情况只需要处理不同queue的queue ownership transfer的问题?
				//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);
				auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];

				std::vector<Queue*> unique_queues_ordered;
				std::ranges::transform(passes_at_same_level_partitioned,
					std::back_inserter(unique_queues_ordered),
					[this](const std::vector<GraphicsPassNode*>& inner_pass_group)-> Queue*
					{
						return pass_to_queue_info.at(inner_pass_group.front()).first;
					}
				);

				auto cur_pass_pos_in_cur_level = std::ranges::find(passes_at_same_levels, cur_pass);
				auto cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);

				ASSERT_WITH_MSG(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
				ASSERT_WITH_MSG(cur_pass_pos_in_cur_level != passes_at_same_levels.end(),
					"Current pass is not found at the level!");

				auto sor_pass = cur_pass;
				auto tar_pass = cur_pass;

				//如果当前pass不是这个level中第一个pass
				if (cur_pass_pos_in_cur_level != passes_at_same_levels.begin())
				{
					// Get the iterator to the previous element
					auto previous_pass_itr = std::prev(cur_pass_pos_in_cur_level);
					// Access the previous element
					sor_pass = *previous_pass_itr;

					//// Element found in the set
					//std::size_t index = std::distance(unique_queues_ordered.begin(), cur_queue_itr);
					//uint64_t    tar_wait_value = index;             //1
					//uint64_t    tar_signal_value = index + 1;       //2

					//uint64_t    sor_wait_value = index - 1;         //0
					//uint64_t    sor_signal_value = index;           //1

					Queue* sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					Queue* tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
					auto& target_let = cur_let;
					auto underlying_buf_handle = cur_let.GetUnderlyingRsrcHandle();

					auto founded_buf_outlet_usage = std::ranges::find_if(sor_pass->outs_buf,
						[&underlying_buf_handle](
							const RsrcOutlet<BufUsage>& outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});

					auto founded_buf_inlet_usage = std::ranges::find_if(sor_pass->ins_buf,
						[&underlying_buf_handle](
							const RsrcInlet<BufUsage>& inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_buf_handle;
						});
					BufUsage* p_sor_buf_usage = nullptr;
					if (founded_buf_outlet_usage != sor_pass->outs_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_outlet_usage->usage);
					}
					if (founded_buf_inlet_usage != sor_pass->ins_buf.end())
					{
						p_sor_buf_usage = &(founded_buf_inlet_usage->usage);
					}


					BufUsage& sor_buf_usage = *p_sor_buf_usage;
					BufUsage& tar_buf_usage = target_let.GetRsrcUsage();


					//Source pass     ------------------------->       Cur pass
					//Sync primitive
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

					const SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
					const WaitValue tar_wait_val{ sync_sema->GetLastValue() + 1 };

					SemInsertInfoSafe sor_insert_info(
						sor_signal_val,
						sor_buf_usage.sync_info.stage_mask
					);

					SemInsertInfoSafe tar_insert_info{
						tar_wait_val,
						tar_buf_usage.sync_info.stage_mask
					};

					//CASE 1: previous pass and current pass in a same queue 
					if (sor_pass_queue_ptr == tar_pass_queue_ptr)
					{
						//because we use barrier, so only one barrier is needed.
						tar_pass->InsertSameQueueSyncInfo(
							sor_pass,
							tar_pass,
							tar_pass_queue_ptr,
							sor_buf_usage.GetSynInfo(),
							tar_buf_usage.GetSynInfo(),
							target_let.GetUnderlyingRsrcHandle()
						);
					}
					//CASE 2: DIFF queue situation (need queue ownership transfer!)
					else
					{
						sor_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_buf_usage.GetSynInfo(),
							tar_buf_usage.GetSynInfo(),
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);

						tar_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_buf_usage.GetSynInfo(),
							tar_buf_usage.GetSynInfo(),
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);
					}
				}
			}
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::SyncPrimitivesInsertionInletTexture(GraphicsPassNode* const cur_pass)
{
	auto& swap_imgs_finshed_using_by_cur_pass = vec_swap_imgs_finshed_using.back().first;
	auto cur_level = cur_pass->cur_level;
	//处理inlet tex

	for (auto& cur_let : cur_pass->ins_tex)
	{
		VirtualTexture& curlet_underlying_rsrc = cur_pass->GetVRsrcFromRsrcHandle(cur_let.GetUnderlyingRsrcHandle());
		std::variant<TexUsage, AttachUsage>& curlet_usage = cur_let.GetRsrcUsage();

		IRsrcUsage::RsrcOrigin curlet_origin =
			std::visit([&](auto& variant_usage) -> IRsrcUsage::RsrcOrigin
				{
					return variant_usage.origin;
				}, curlet_usage);

		ImgSyncInfo curlet_sync_info =
			std::visit([&](auto& variant_usage) -> ImgSyncInfo
				{
					return variant_usage.GetSynInfo();
				}, curlet_usage);

		//直接在当前pass建立的资源，同步原语插入：
		if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
		{
			auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
			cur_pass->InsertSyncInfoForInitalUsage(
				source_sync,
				curlet_sync_info,
				cur_let.GetUnderlyingRsrcHandle()
			);
		}


		//来自rendergraph外部的
		if (IRsrcUsage::RsrcOrigin::FromOutSide == curlet_origin)
		{
			auto first_pass = curlet_underlying_rsrc.passes_access_this_rsrc.front().first;
			if (first_pass == cur_pass)
			{
				if (curlet_underlying_rsrc.p_rsrc)
				{
					const auto& tex_img = curlet_underlying_rsrc.p_rsrc->GetTextureImage();
					auto& sem_wrapper = tex_img->GetSemUsedToTransfer();

					if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState()) //说明还没加载完成，这样的话，我们需要插入带有semaphore的同步原语
					{
						auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();

						SemInsertInfoSafe sem_insert_info(WaitValue{ sem_wrapper->GetLastValue() },
							curlet_sync_info.stage_mask);

						cur_pass->InsertSyncInfoForInitalLoad(
							source_sync,
							curlet_sync_info,
							cur_let.GetUnderlyingRsrcHandle(),
							sem_wrapper,
							sem_insert_info
						);

						if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
						{
							{
								//maybe some previous frames is using the current image , so wait
								auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->
									swap_img_rdy_4_rendering_helper_sem;
								const auto wait_value = WaitValue{ rdy_4_ren_helper_sem->GetLastValue() }; //
								cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(),
									SemInsertInfoSafe(
										wait_value, curlet_sync_info.stage_mask));
								//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
							}
							{
								//present engine is shitting on the image, wait for it done using it.
								auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->
									present_finished_helper_sem;
								const auto wait_value = WaitValue{
									present_engine_finshed_presenting->GetLastValue()
								}; //
								cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(),
									SemInsertInfoSafe{
										wait_value, curlet_sync_info.stage_mask
									});
							}
							{
								if (Present != cur_pass->GetRenderpassType())
								{
									auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(
										std::make_shared<BinarySemWrapper>(device_manager));
									constexpr auto signal_value = SignalValue{ 1 }; //
									cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(),
										SemInsertInfoSafe{
											signal_value, curlet_sync_info.stage_mask
										});
								}
							}
						}
					}
					else //已经加载过了，
					{
						auto source_sync = ImgSyncInfo{
							.access_mask = vk::AccessFlagBits2::eNone,
							.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
							.layout_inpass = vk::ImageLayout::eUndefined,
							.img_subrange = std::nullopt
						};
						cur_pass->InsertSyncInfoForInitalUsage(
							source_sync,
							curlet_sync_info,
							cur_let.GetUnderlyingRsrcHandle()
						);

						if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
						{
							{
								//maybe some previous frames is using the current image , so wait
								auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->
									swap_img_rdy_4_rendering_helper_sem;
								const auto wait_value = WaitValue{ rdy_4_ren_helper_sem->GetLastValue() }; //
								cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(),
									SemInsertInfoSafe(
										wait_value, curlet_sync_info.stage_mask));
								//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
							}
							{
								//present engine is shitting on the image, wait for it done using it.
								auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->
									present_finished_helper_sem;
								const auto wait_value = WaitValue{
									present_engine_finshed_presenting->GetLastValue()
								}; //
								cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(),
									SemInsertInfoSafe{
										wait_value, curlet_sync_info.stage_mask
									});
							}
							{
								if (Present != cur_pass->GetRenderpassType())
								{
									auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(
										std::make_shared<BinarySemWrapper>(device_manager));
									constexpr auto signal_value = SignalValue{ 1 }; //
									cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(),
										SemInsertInfoSafe{
											signal_value, curlet_sync_info.stage_mask
										});
								}
							}
						}
					}
				}
				else
				{
					for (auto& p_rsrc : *curlet_underlying_rsrc.p_rsrcs.value())
					{
						const auto& tex_img = p_rsrc->GetTextureImage();
						auto& sem_wrapper = tex_img->GetSemUsedToTransfer();
						if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState())
							//说明还没加载完成，这样的话，我们需要插入带有semaphore的同步原语
						{
							SemInsertInfoSafe sem_insert_info
							{
								WaitValue{sem_wrapper->GetLastValue()},
								curlet_sync_info.stage_mask
							};

							auto source_sync = p_rsrc->GetSynInfoOnLoad();
							cur_pass->InsertSyncInfoForInitalLoad(
								source_sync,
								curlet_sync_info,
								cur_let.GetUnderlyingRsrcHandle(),
								sem_wrapper,
								sem_insert_info
							);
						}
						else //已经加载过了，
						{
							auto source_sync = ImgSyncInfo{
								.access_mask = vk::AccessFlagBits2::eNone,
								.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
								.layout_inpass = vk::ImageLayout::eUndefined,
								.img_subrange = std::nullopt
							};
							cur_pass->InsertSyncInfoForInitalUsage(
								source_sync,
								curlet_sync_info,
								cur_let.GetUnderlyingRsrcHandle()
							);
						}
					}
				}
			}
		}

		//拿到所有使用该资源的pass，对于之前的每一个layer，进行同步原语插入。比如当前layer是5，我们首先需要先处理layer=0,1,2,3,4
		for (auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.
			level_to_passes_attached_to_partitioned)
		{
			//如果前面的layer i 出现了对某一资源同时access的情况，
			//layer i 可能含有多个执行在不同queue上的passes，不过他们是按照分块排序的，这种情况下，我们只需要在最后一块中的所有pass和当前pass之间插入同步即可
			if (level < cur_level)
			{
				const auto& last_passes_group = passes_at_same_level_partitioned.back();
				auto last_queue = pass_to_queue_info.at(last_passes_group.back());
				auto queue_which_cur_pass_execute = pass_to_queue_info.at(cur_pass);

				if (last_queue == queue_which_cur_pass_execute)
				{
					auto last_pass_executing_on_last_queue_group = last_passes_group.back();
					auto& target_let = cur_let;

					auto tar_pass = cur_pass;
					auto sor_pass = last_pass_executing_on_last_queue_group;

					auto underlying_tex_handle = cur_let.GetUnderlyingRsrcHandle();
					auto founded_tex_outlet_usage = std::ranges::find_if(sor_pass->outs_tex,
						[&underlying_tex_handle](
							const RsrcOutlet<std::variant<
							TexUsage, AttachUsage>>&outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});

					auto founded_tex_inlet_usage = std::ranges::find_if(sor_pass->ins_tex,
						[&underlying_tex_handle](
							const RsrcInlet<std::variant<
							TexUsage, AttachUsage>>&inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});
					std::variant<TexUsage, AttachUsage>* p_sor_buf_usage = nullptr;
					if (founded_tex_outlet_usage != sor_pass->outs_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_outlet_usage->usage);
					}
					if (founded_tex_inlet_usage != sor_pass->ins_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_inlet_usage->usage);
					}


					std::variant<TexUsage, AttachUsage>& sor_tex_usage = *p_sor_buf_usage;
					std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();


					ImgSyncInfo sor_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, sor_tex_usage);

					ImgSyncInfo tar_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, tar_tex_usage);

					auto underlying_tex = target_let.GetUnderlyingRsrcHandle();

					//几个pass执行的Queue相同，直接用same queue sync info
					//because we use barrier, so only one barrier is needed.
					cur_pass->InsertSameQueueSyncInfo(
						sor_pass,
						tar_pass,
						queue_which_cur_pass_execute.first,
						sor_tex_sync_ino,
						tar_tex_sync_ino,
						underlying_tex
					);
				}
				else
				{
					const auto last_pass = last_passes_group.back();
					const auto tar_pass = cur_pass;
					const auto sor_pass = last_pass;

					const auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					const auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;


					auto& target_let = cur_let;
					auto underlying_tex_handle = target_let.GetUnderlyingRsrcHandle();

					auto founded_tex_outlet_usage = std::ranges::find_if(sor_pass->outs_tex,
						[&underlying_tex_handle](
							const RsrcOutlet<std::variant<
							TexUsage, AttachUsage>>&outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});

					auto founded_tex_inlet_usage = std::ranges::find_if(sor_pass->ins_tex,
						[&underlying_tex_handle](
							const RsrcInlet<std::variant<
							TexUsage, AttachUsage>>&inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});
					std::variant<TexUsage, AttachUsage>* p_sor_buf_usage = nullptr;
					if (founded_tex_outlet_usage != sor_pass->outs_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_outlet_usage->usage);
					}
					if (founded_tex_inlet_usage != sor_pass->ins_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_inlet_usage->usage);
					}

					std::variant<TexUsage, AttachUsage>& sor_tex_usage = *p_sor_buf_usage;
					std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();


					ImgSyncInfo sor_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, sor_tex_usage);

					ImgSyncInfo tar_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, tar_tex_usage);

					//Source pass     ------------------------->       Cur pass
					//Sync primitive
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });
					const uint64_t sor_signal_val = sync_sema->GetLastValue() + 1;
					const uint64_t tar_wait_val = sync_sema->GetLastValue() + 1;


					SemInsertInfoSafe sor_insert_info{
						SignalValue{sync_sema->GetLastValue() + 1},
						sor_tex_sync_ino.stage_mask
					};
					SemInsertInfoSafe tar_insert_info{
						WaitValue(tar_wait_val),
						tar_tex_sync_ino.stage_mask
					};

					sor_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_tex_sync_ino,
						tar_tex_sync_ino,
						underlying_tex_handle,
						sor_insert_info,
						tar_insert_info
					);

					tar_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_tex_sync_ino,
						tar_tex_sync_ino,
						underlying_tex_handle,
						sor_insert_info,
						tar_insert_info
					);
				}
			}
			//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
			if (level == cur_level)
			{
				//passes_at_same_levels排布一定是 按照queue 分块的
				// 比如passes_at_same_levels是   pass0 , pass1 , pass4, pass5 , pass7 , pass9
				// 块0  queue3 ：|pass0，pass1，pass4| ;  块1  queue 2 ：|pass5，pass7，pass9|

				//在同一个level中决定如何同步：在同一level中的pass应该只会出现同时读一个资源的情况，这种情况只需要处理不同queue的queue ownership transfer的问题?
				//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);
				auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];


				std::vector<Queue*> unique_queues_ordered;
				std::ranges::transform(passes_at_same_level_partitioned,
					std::back_inserter(unique_queues_ordered),
					[this](const std::vector<GraphicsPassNode*>& inner_pass_group)-> Queue*
					{
						return pass_to_queue_info.at(inner_pass_group.front()).first;
					}
				);

				auto cur_pass_pos_in_cur_level = std::ranges::find(passes_at_same_levels, cur_pass);
				auto cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);


				ASSERT_WITH_MSG(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
				ASSERT_WITH_MSG(cur_pass_pos_in_cur_level != passes_at_same_levels.end(),
					"Current pass is not found at the level!");

				auto sor_pass = cur_pass;
				auto tar_pass = cur_pass;

				//如果当前pass不是这个level中第一个pass
				if (cur_pass_pos_in_cur_level != passes_at_same_levels.begin())
				{
					// Get the iterator to the previous element
					auto previous_pass_itr = std::prev(cur_pass_pos_in_cur_level);
					// Access the previous element
					sor_pass = *previous_pass_itr;

					// Element found in the set
					//std::size_t index = std::distance(unique_queues_ordered.begin(), cur_queue_itr);
					//uint64_t    tar_wait_value = index;             //1
					//uint64_t    tar_signal_value = index + 1;       //2

					//uint64_t    sor_wait_value = index - 1;         //0
					//uint64_t    sor_signal_value = index;           //1

					auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
					auto& target_let = cur_let;
					auto underlying_tex_handle = cur_let.GetUnderlyingRsrcHandle();

					auto founded_tex_outlet_usage = std::ranges::find_if(sor_pass->outs_tex,
						[&underlying_tex_handle](
							const RsrcOutlet<std::variant<
							TexUsage, AttachUsage>>&outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});

					auto founded_tex_inlet_usage = std::ranges::find_if(sor_pass->ins_tex,
						[&underlying_tex_handle](
							const RsrcInlet<std::variant<
							TexUsage, AttachUsage>>&inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});
					std::variant<TexUsage, AttachUsage>* p_sor_buf_usage = nullptr;
					if (founded_tex_outlet_usage != sor_pass->outs_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_outlet_usage->usage);
					}
					if (founded_tex_inlet_usage != sor_pass->ins_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_inlet_usage->usage);
					}

					std::variant<TexUsage, AttachUsage>& sor_tex_usage = *p_sor_buf_usage;
					std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

					ImgSyncInfo sor_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, sor_tex_usage);

					ImgSyncInfo tar_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, tar_tex_usage);

					//Source pass     ------------------------->       Cur pass
					//Sync primitive
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

					SemInsertInfoSafe sor_insert_info{
						SignalValue(sync_sema->GetLastValue() + 1),
						sor_tex_sync_ino.stage_mask
					};

					SemInsertInfoSafe tar_insert_info{
						WaitValue(sync_sema->GetLastValue() + 1),
						tar_tex_sync_ino.stage_mask
					};


					//CASE 1: previous pass and current pass in a same queue 
					if (sor_pass_queue_ptr == tar_pass_queue_ptr)
					{
						//because we use barrier, so only one barrier is needed.
						tar_pass->InsertSameQueueSyncInfo(
							sor_pass,
							tar_pass,
							tar_pass_queue_ptr,
							sor_tex_sync_ino,
							tar_tex_sync_ino,
							target_let.GetUnderlyingRsrcHandle()
						);
					}
					//CASE 2: DIFF queue situation (need queue ownership transfer!)
					else
					{
						sor_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_tex_sync_ino,
							tar_tex_sync_ino,
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);

						tar_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_tex_sync_ino,
							tar_tex_sync_ino,
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);
					}
				}
			}
		}
	}
}

void Anni::RenderGraphV1::DependencyGraph::SyncPrimitivesInsertionOutletTexture(GraphicsPassNode* const cur_pass)
{
	auto& swap_imgs_finshed_using_by_cur_pass = vec_swap_imgs_finshed_using.back().first;

	auto cur_level = cur_pass->cur_level;
	//处理inlet tex
	for (auto& cur_let : cur_pass->outs_tex)
	{
		VirtualTexture& curlet_underlying_rsrc = cur_pass->GetVRsrcFromRsrcHandle(cur_let.GetUnderlyingRsrcHandle());
		std::variant<TexUsage, AttachUsage>& curlet_usage = cur_let.GetRsrcUsage();

		IRsrcUsage::RsrcOrigin curlet_origin =
			std::visit([](auto& variant_usage) -> IRsrcUsage::RsrcOrigin
				{
					return variant_usage.origin;
				}, curlet_usage);

		ImgSyncInfo curlet_sync_info =
			std::visit([](auto& variant_usage) -> ImgSyncInfo
				{
					return variant_usage.GetSynInfo();
				}, curlet_usage);


		//std::shared_ptr<BinarySemWrapper>& curlet_available_sem =
		//	std::visit([](auto& variant_usage) -> std::shared_ptr<BinarySemWrapper>&
		//		{
		//			return variant_usage.GetAvailableSem();
		//		}, curlet_usage);


		//直接在当前pass建立的资源，同步原语插入：
		if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
		{
			auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
			cur_pass->InsertSyncInfoForInitalUsage(
				source_sync,
				curlet_sync_info,
				cur_let.GetUnderlyingRsrcHandle()
			);
		}

		if (IRsrcUsage::RsrcOrigin::FromOutSide == curlet_origin)
		{
			auto first_pass = curlet_underlying_rsrc.passes_access_this_rsrc.front().first;
			if (first_pass == cur_pass)
			{
				if (curlet_underlying_rsrc.p_rsrc)
				{
					const auto& tex_img = curlet_underlying_rsrc.p_rsrc->GetTextureImage();
					auto& sem_wrapper = tex_img->GetSemUsedToTransfer();

					if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState()) //说明还没加载完成，这样的话，我们需要插入带有semaphore的同步原语
					{
						auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
						SemInsertInfoSafe sem_insert_info
						{
							WaitValue(sem_wrapper->GetLastValue()),
							curlet_sync_info.stage_mask
						};

						cur_pass->InsertSyncInfoForInitalLoad(
							source_sync,
							curlet_sync_info,
							cur_let.GetUnderlyingRsrcHandle(),
							sem_wrapper,
							sem_insert_info
						);


						if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
						{
							{
								//maybe some previous frames is using the current image , so wait
								auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->
									swap_img_rdy_4_rendering_helper_sem;
								const auto wait_value = WaitValue{ rdy_4_ren_helper_sem->GetLastValue() }; //
								cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(),
									SemInsertInfoSafe(
										wait_value, curlet_sync_info.stage_mask));
								//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
							}
							{
								//present engine is shitting on the image, wait for it done using it.
								auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->
									present_finished_helper_sem;
								const auto wait_value = WaitValue{
									present_engine_finshed_presenting->GetLastValue()
								}; //
								cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(),
									SemInsertInfoSafe{
										wait_value, curlet_sync_info.stage_mask
									});
							}
							{
								if (Present != cur_pass->GetRenderpassType())
								{
									auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(
										std::make_shared<BinarySemWrapper>(device_manager));
									constexpr auto signal_value = SignalValue{ 1 }; //
									cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(),
										SemInsertInfoSafe{
											signal_value, curlet_sync_info.stage_mask
										});
								}
							}
						}
					}
					else //已经加载过了，
					{
						auto source_sync = ImgSyncInfo{
							.access_mask = vk::AccessFlagBits2::eNone,
							.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
							.layout_inpass = vk::ImageLayout::eUndefined,
							.img_subrange = std::nullopt
						};
						cur_pass->InsertSyncInfoForInitalUsage(
							source_sync,
							curlet_sync_info,
							cur_let.GetUnderlyingRsrcHandle()
						);


						if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
						{
							{
								//maybe some previous frames is using the current image , so wait
								auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->
									swap_img_rdy_4_rendering_helper_sem;
								const auto wait_value = WaitValue{ rdy_4_ren_helper_sem->GetLastValue() }; //
								cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(),
									SemInsertInfoSafe(
										wait_value, curlet_sync_info.stage_mask));
								//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
							}
							{
								//present engine is shitting on the image, wait for it done using it.
								auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->
									present_finished_helper_sem;
								const auto wait_value = WaitValue{
									present_engine_finshed_presenting->GetLastValue()
								}; //
								cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(),
									SemInsertInfoSafe{
										wait_value, curlet_sync_info.stage_mask
									});
							}
							{
								if (Present != cur_pass->GetRenderpassType())
								{
									auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(
										std::make_shared<BinarySemWrapper>(device_manager));
									constexpr auto signal_value = SignalValue{ 1 }; //
									cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(),
										SemInsertInfoSafe{
											signal_value, curlet_sync_info.stage_mask
										});
								}
							}
						}
					}
				}
				else
				{
					for (auto& p_rsrc : *curlet_underlying_rsrc.p_rsrcs.value())
					{
						const auto& tex_img = p_rsrc->GetTextureImage();
						auto& sem_wrapper = tex_img->GetSemUsedToTransfer();
						if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState())
							//说明还没加载完成，这样的话，我们需要插入带有semaphore的同步原语
						{
							SemInsertInfoSafe sem_insert_info
							{
								WaitValue(sem_wrapper->GetLastValue()),
								curlet_sync_info.stage_mask
							};

							auto source_sync = p_rsrc->GetSynInfoOnLoad();
							cur_pass->InsertSyncInfoForInitalLoad(
								source_sync,
								curlet_sync_info,
								cur_let.GetUnderlyingRsrcHandle(),
								sem_wrapper,
								sem_insert_info
							);
						}
						else //已经加载过了，
						{
							auto source_sync = ImgSyncInfo{
								.access_mask = vk::AccessFlagBits2::eNone,
								.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
								.layout_inpass = vk::ImageLayout::eUndefined,
								.img_subrange = std::nullopt
							};
							cur_pass->InsertSyncInfoForInitalUsage(
								source_sync,
								curlet_sync_info,
								cur_let.GetUnderlyingRsrcHandle()
							);
						}
					}
				}
			}
		}


		//拿到所有使用该资源的pass，对于之前的每一个layer，进行同步原语插入。比如当前layer是5，我们首先需要先处理layer=0,1,2,3,4
		for (auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.
			level_to_passes_attached_to_partitioned)
		{
			//如果前面的layer i 出现了对某一资源同时access的情况，
			//layer i 可能含有多个执行在不同queue上的passes，不过他们是按照分块排序的，这种情况下，我们只需要在最后一块中的所有pass和当前pass之间插入同步即可
			if (level < cur_level)
			{
				const auto& last_passes_group = passes_at_same_level_partitioned.back();
				auto last_queue = pass_to_queue_info.at(last_passes_group.back());
				auto queue_which_cur_pass_execute = pass_to_queue_info.at(cur_pass);

				if (last_queue == queue_which_cur_pass_execute)
				{
					auto last_pass_executing_on_last_queue_group = last_passes_group.back();
					auto& target_let = cur_let;

					auto tar_pass = cur_pass;
					auto sor_pass = last_pass_executing_on_last_queue_group;


					auto underlying_tex_handle = target_let.GetUnderlyingRsrcHandle();
					auto founded_tex_outlet_usage = std::ranges::find_if(sor_pass->outs_tex,
						[&underlying_tex_handle](
							const RsrcOutlet<std::variant<
							TexUsage, AttachUsage>>&outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});

					auto founded_tex_inlet_usage = std::ranges::find_if(sor_pass->ins_tex,
						[&underlying_tex_handle](
							const RsrcInlet<std::variant<
							TexUsage, AttachUsage>>&inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});
					std::variant<TexUsage, AttachUsage>* p_sor_buf_usage = nullptr;
					if (founded_tex_outlet_usage != sor_pass->outs_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_outlet_usage->usage);
					}
					if (founded_tex_inlet_usage != sor_pass->ins_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_inlet_usage->usage);
					}


					std::variant<TexUsage, AttachUsage>& sor_tex_usage = *p_sor_buf_usage;
					std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

					ImgSyncInfo sor_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, sor_tex_usage);

					ImgSyncInfo tar_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, tar_tex_usage);

					auto underlying_tex = target_let.GetUnderlyingRsrcHandle();

					//几个pass执行的Queue相同，直接用same queue sync info
					//because we use barrier, so only one barrier is needed.
					cur_pass->InsertSameQueueSyncInfo(
						sor_pass,
						tar_pass,
						queue_which_cur_pass_execute.first,
						sor_tex_sync_ino,
						tar_tex_sync_ino,
						underlying_tex
					);
				}
				else
				{
					const auto last_pass = last_passes_group.back();
					const auto tar_pass = cur_pass;
					const auto sor_pass = last_pass;

					const auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					const auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;

					auto& target_let = cur_let;

					auto underlying_tex_handle = target_let.GetUnderlyingRsrcHandle();
					auto founded_tex_outlet_usage = std::ranges::find_if(sor_pass->outs_tex,
						[&underlying_tex_handle](
							const RsrcOutlet<std::variant<
							TexUsage, AttachUsage>>&outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});

					auto founded_tex_inlet_usage = std::ranges::find_if(sor_pass->ins_tex,
						[&underlying_tex_handle](
							const RsrcInlet<std::variant<
							TexUsage, AttachUsage>>&inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});
					std::variant<TexUsage, AttachUsage>* p_sor_buf_usage = nullptr;
					if (founded_tex_outlet_usage != sor_pass->outs_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_outlet_usage->usage);
					}
					if (founded_tex_inlet_usage != sor_pass->ins_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_inlet_usage->usage);
					}


					std::variant<TexUsage, AttachUsage>& sor_tex_usage = *p_sor_buf_usage;
					std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

					ImgSyncInfo sor_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, sor_tex_usage);

					ImgSyncInfo tar_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, tar_tex_usage);

					//Source pass     ------------------------->       Cur pass
					//Sync primitive
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

					SemInsertInfoSafe sor_insert_info{
						SignalValue(sync_sema->GetLastValue() + 1),
						sor_tex_sync_ino.stage_mask
					};
					SemInsertInfoSafe tar_insert_info{
						WaitValue(sync_sema->GetLastValue() + 1),
						tar_tex_sync_ino.stage_mask
					};

					sor_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_tex_sync_ino,
						tar_tex_sync_ino,
						underlying_tex_handle,
						sor_insert_info,
						tar_insert_info
					);

					tar_pass->InsertDiffQueueSyncInfoTimeline(
						sor_pass,
						tar_pass,
						sync_sema,
						sor_pass_queue_ptr,
						tar_pass_queue_ptr,
						sor_tex_sync_ino,
						tar_tex_sync_ino,
						underlying_tex_handle,
						sor_insert_info,
						tar_insert_info
					);
				}
			}
			//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
			if (level == cur_level)
			{
				//passes_at_same_levels排布一定是 按照queue 分块的
				// 比如passes_at_same_levels是   pass0 , pass1 , pass4, pass5 , pass7 , pass9
				// 块0  queue3 ：|pass0，pass1，pass4| ;  块1  queue 2 ：|pass5，pass7，pass9|

				//在同一个level中决定如何同步：在同一level中的pass应该只会出现同时读一个资源的情况，这种情况只需要处理不同queue的queue ownership transfer的问题?
				//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);
				auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];


				std::vector<Queue*> unique_queues_ordered;
				std::transform(passes_at_same_level_partitioned.begin(), passes_at_same_level_partitioned.end(),
					std::back_inserter(unique_queues_ordered),
					[this](const std::vector<GraphicsPassNode*>& inner_pass_group)-> Queue*
					{
						return pass_to_queue_info.at(inner_pass_group.front()).first;
					}
				);

				auto cur_pass_pos_in_cur_level = std::find(passes_at_same_levels.begin(), passes_at_same_levels.end(),
					cur_pass);
				auto cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);


				ASSERT_WITH_MSG(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
				ASSERT_WITH_MSG(cur_pass_pos_in_cur_level != passes_at_same_levels.end(),
					"Current pass is not found at the level!");

				auto sor_pass = cur_pass;
				auto tar_pass = cur_pass;

				//如果当前pass不是这个level中第一个pass
				if (cur_pass_pos_in_cur_level != passes_at_same_levels.begin())
				{
					// Get the iterator to the previous element
					auto previous_pass_itr = std::prev(cur_pass_pos_in_cur_level);
					// Access the previous element
					sor_pass = *previous_pass_itr;

					// Element found in the set
					//std::size_t index = std::distance(unique_queues_ordered.begin(), cur_queue_itr);
					//uint64_t    tar_wait_value = index;             //1
					//uint64_t    tar_signal_value = index + 1;       //2

					//uint64_t    sor_wait_value = index - 1;         //0
					//uint64_t    sor_signal_value = index;           //1

					auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
					auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
					auto& target_let = cur_let;

					auto underlying_tex_handle = target_let.GetUnderlyingRsrcHandle();
					auto founded_tex_outlet_usage = std::ranges::find_if(sor_pass->outs_tex,
						[&underlying_tex_handle](
							const RsrcOutlet<std::variant<
							TexUsage, AttachUsage>>&outlet_buf)
						{
							return outlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});

					auto founded_tex_inlet_usage = std::ranges::find_if(sor_pass->ins_tex,
						[&underlying_tex_handle](
							const RsrcInlet<std::variant<
							TexUsage, AttachUsage>>&inlet_buf)
						{
							return inlet_buf.rsrc_handle ==
								underlying_tex_handle;
						});
					std::variant<TexUsage, AttachUsage>* p_sor_buf_usage = nullptr;
					if (founded_tex_outlet_usage != sor_pass->outs_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_outlet_usage->usage);
					}
					if (founded_tex_inlet_usage != sor_pass->ins_tex.end())
					{
						p_sor_buf_usage = &(founded_tex_inlet_usage->usage);
					}

					std::variant<TexUsage, AttachUsage>& sor_tex_usage = *p_sor_buf_usage;
					std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

					ImgSyncInfo sor_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, sor_tex_usage);

					ImgSyncInfo tar_tex_sync_ino =
						std::visit([&](auto& variant_usage) -> ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, tar_tex_usage);

					//Source pass     ------------------------->       Cur pass
					//Sync primitive
					auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

					SemInsertInfoSafe sor_insert_info{
						SignalValue(sync_sema->GetLastValue() + 1),
						sor_tex_sync_ino.stage_mask
					};
					SemInsertInfoSafe tar_insert_info{
						WaitValue{
							sync_sema->GetLastValue() + 1
						},
						tar_tex_sync_ino.stage_mask
					};

					//CASE 1: previous pass and current pass in a same queue 
					if (sor_pass_queue_ptr == tar_pass_queue_ptr)
					{
						//because we use barrier, so only one barrier is needed.
						tar_pass->InsertSameQueueSyncInfo(
							sor_pass,
							tar_pass,
							tar_pass_queue_ptr,
							sor_tex_sync_ino,
							tar_tex_sync_ino,
							target_let.GetUnderlyingRsrcHandle()
						);
					}
					//CASE 2: DIFF queue situation (need queue ownership transfer!)
					else
					{
						sor_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_tex_sync_ino,
							tar_tex_sync_ino,
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);

						tar_pass->InsertDiffQueueSyncInfoTimeline(
							sor_pass,
							tar_pass,
							sync_sema,
							sor_pass_queue_ptr,
							tar_pass_queue_ptr,
							sor_tex_sync_ino,
							tar_tex_sync_ino,
							target_let.GetUnderlyingRsrcHandle(),
							sor_insert_info,
							tar_insert_info
						);
					}
				}
			}
		}
	}
}
