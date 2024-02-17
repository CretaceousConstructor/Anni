#pragma once

#include "SyncInfo.h"
#include "VkRenderpassBaseRG.h"
#include "VkRsrcUsageInfoRG.h"
#include "QueueManager.h"
#include "VkShaderFactory.h"
#include "VirtualBuffer.h"
#include "VirtualTexture.h"
#include "DesSetLayoutManager.h"
#include "ImgViewWrapper.h"
#include "SamplerWrapper.h"
#include "Renderable.h"
#include "RsrcUsage.h"
#include "RGSyncInfo.h"
#include "VirtualRsrcAndUsage.h"

#include "Lets.h"
#include "IPassNode.h"
#include "DeferedGeometryPass.h"
#include <variant>
#include <vector>
#include <ranges>
#include <functional>


namespace Anni::RenderGraphV1
{
	class GraphicsPassNode;
}



namespace Anni::RenderGraphV1
{

	class DependencyGraph
	{
	public:
		DependencyGraph(
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			QueueManager& queue_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			DescriptorAllocatorGrowable& descriptor_allocator_,
			VkTimelineSemPoolUnsafe& semaphore_pool_,
			BufferFactory& buf_fac_,
			VkTextureFactory& tex_fac_,
			VkShaderFactory& shader_fac_,
			GFXPipelineCIBuilder& gfx_pipelineCI_builder_)
			:
			device_manager(device_manager_),
			swapchain_manager(swapchain_manager_),
			queue_manager(queue_manager_),
			descriptor_set_layout_manager(descriptor_set_layout_manager_),
			descriptor_allocator(descriptor_allocator_),
			semaphore_pool(semaphore_pool_),
			buf_fac(buf_fac_),
			tex_fac(tex_fac_),
			shader_fac(shader_fac_),
			gfx_pipelineCI_builder(gfx_pipelineCI_builder_)
		{
		}

	private:
		uint64_t cur_frame;
		uint32_t img_index;
	public:
		void SetCurFrameAndImgIndex(
			uint64_t cur_frame_,
			uint32_t img_index_
		)
		{
			cur_frame = cur_frame_;
			img_index = img_index_;
		}




		DependencyGraph() = delete;
		~DependencyGraph() = default;

		DependencyGraph(const DependencyGraph&) = delete;
		DependencyGraph& operator=(const DependencyGraph&) = delete;
		DependencyGraph(DependencyGraph&&) = delete;
		DependencyGraph& operator=(DependencyGraph&&) = delete;


		template <typename P>
		P& AddGfxPassNode(std::string name);

		template <>
		DeferedGeometryPass& AddGfxPassNode<DeferedGeometryPass>(std::string name)
		{
			auto gfx_node =
				std::make_unique<DeferedGeometryPass>(
					name,
					device_manager,
					swapchain_manager,
					descriptor_allocator,
					shader_fac,
					descriptor_allocator,
					gfx_pipelineCI_builder,
					this->rg_buffers_map,
					this->rg_textures_map
				);
			pass_nodes.push_back(std::move(gfx_node));
			auto& result = *pass_nodes.back();
			DeferedGeometryPass& derived_pass = static_cast<DeferedGeometryPass&>(result);

			return derived_pass;
		}


		void Compile()
		{
			//TODO: Trim all invalid passes and unused resources
			//->Render graph compilation starts.
			//************************************
			topologically_sorted_nodes.clear();        //�������ִ��˳�� 
			GeneratePassNodeDepen();                   //����In,Out��������ȷ�������ƶϳ�����˳��
			TopologicalDFSSort();                      //����
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




		void CmdBufExecuteOnQueue(
			GraphicsPassNode* p_pass, uint32_t cur_frame, vk::CommandBuffer cmd_buf, Queue* execution_queue, VkSemaphore frame_num_sem,
			std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_infos,
			std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_infos
		)
		{
			//wait and signal semaphore
			const uint64_t wait_value = cur_frame < (Vk::MAX_FRAMES_OVERLAP - 1) ? 0 : cur_frame - (Vk::MAX_FRAMES_OVERLAP - 1);
			wait_sem_submit_infos.push_back(
				vk::SemaphoreSubmitInfo
				(
					frame_num_sem,
					wait_value,
					vk::PipelineStageFlagBits2::eAllCommands
				)
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



		void CmdBufRecordingAndExecuting(uint32_t img_index, uint32_t cur_frame, vk::Semaphore frame_number_semaphore)
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
			//	for (auto& tex : rg_textures_map)
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
				//�ȹ۲����level�ϵ�pass�費��Ҫ���������Ҫ����ô��Ҫ������level��ִ��˳�����ִ��˳�����ȸ���queue fam���֣��ٸ���queue��ַ���֣�������pass��ַ����

				//����Ҫlevel�ڲ����ֵ����
				if (!if_level_accesses_same_rsrc_multimes[level])
				{
					for (auto p_pass : level_to_all_passes_attached[level])
					{
						//PRESENT PASS SPECIAL TEATMENT
						if (PassType::Present == p_pass->GetRenderpassType())
						{
							p_pass->ResourcesAcquisition(tex_fac, buf_fac);

							std::vector<vk::SemaphoreSubmitInfo> wait_sem_submit_info;
							std::vector<vk::SemaphoreSubmitInfo> signal_sem_submit_info;

							Queue& cur_present_pass_queue = *pass_to_queue_info.at(p_pass).first;
							std::pair<vk::CommandBuffer, vk::Semaphore> cmd_buf_and_sem = cur_present_pass_queue.GetOneBufAndAssociatedSem();

							p_pass->SyncInsertAhead(cmd_buf_and_sem.first, wait_sem_submit_info);
							//p_pass->RecordCommandBuffer(cmd_buf_and_sem.first);
							p_pass->SyncInsertAfter(cmd_buf_and_sem.first, signal_sem_submit_info);


							VULKAN_HPP_ASSERT(1 == p_pass->tex_usages.size(), "Present pass can only have swap image as its only resource.");
							auto& swap_img_usage = p_pass->tex_usages.front();
							auto& swap_img = swap_img_usage.v_rsrc->second.p_rsrc;
							VULKAN_HPP_ASSERT(swap_img->IsSwapTexture(), "Present pass can only have swap image as its only resource.");


							{
								for (const auto& swap_img_finshed_using : vec_swap_imgs_finshed_using.back())  //
								{
									constexpr uint64_t wait_value = 1;
									wait_sem_submit_info.push_back(
										vk::SemaphoreSubmitInfo(
											swap_img_finshed_using->GetRaw(),
											wait_value,
											vk::PipelineStageFlagBits2::eAllCommands
										)
									);
								}
							}


							{
								auto& rdy_4_ren_helper_sem = swap_img->swap_img_rdy_4_rendering_helper_sem;        // timeline sem
								uint64_t signal_val = ++rdy_4_ren_helper_sem->GetLastValue();   //1

								signal_sem_submit_info.push_back(
									vk::SemaphoreSubmitInfo(
										rdy_4_ren_helper_sem->GetRaw(),
										signal_val,
										vk::PipelineStageFlagBits2::eAllCommands                   //
									)
								);

								auto& present_finished_helper_sem = swap_img->present_finished_helper_sem;         //timeline sem
								signal_val = ++present_finished_helper_sem->GetLastValue();   //1

								signal_sem_submit_info.push_back(
									vk::SemaphoreSubmitInfo(
										present_finished_helper_sem->GetRaw(),
										signal_val,
										vk::PipelineStageFlagBits2::eAllCommands                   //
									)
								);
							}


							{
								const uint64_t wait_value = cur_frame < (Vk::MAX_FRAMES_OVERLAP - 1) ? 0 : cur_frame - (Vk::MAX_FRAMES_OVERLAP - 1);
								vk::SemaphoreWaitInfo wait_info;
								wait_info
									.setSemaphores(frame_number_semaphore)
									.setValues(wait_value);

								device_manager.GetLogicalDevice().waitSemaphores(wait_info, UINT64_MAX);

								auto& present_finished_sem = swap_img->present_finished_sem;                    //binary sem��Ҫ��VkWaitTimelineSemaphore����ֹ����ȴ� 
								constexpr uint64_t wait_val = 1;
								wait_sem_submit_info.push_back(
									vk::SemaphoreSubmitInfo(
										present_finished_sem->GetRaw(),
										wait_val,
										vk::PipelineStageFlagBits2::eAllCommands
									)
								);

								constexpr uint64_t signal_value = 1;                                           //binary sem��Ҫ��VkWaitTimelineSemaphore����ֹ����ȴ�
								signal_sem_submit_info.push_back(
									vk::SemaphoreSubmitInfo(
										swap_img->ready_for_present_sem->GetRaw(),
										signal_value,
										vk::PipelineStageFlagBits2::eAllCommands                   //
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
							VULKAN_HPP_ASSERT(present_result == vk::Result::eSuccess, "failed to present swap chain image!");
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
							std::pair<VkCommandBuffer, VkSemaphore> cmd_buf_and_sem = cur_pass_queue.GetOneBufAndAssociatedSem();

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

							CmdBufExecuteOnQueue(p_pass, cur_frame, cmd_buf_and_sem.first, &cur_pass_queue, frame_number_semaphore, wait_sem_submit_info, signal_sem_submit_info);
						}
					}
				}
				else
				{
					assert(false, "not yet implemented");

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
				//TODO:�����ҵ�queue cap��ǿ��queue��Ȼ������finalize����ʱ�����ѡһ��queue
				if (!p_queue_used_to_finalize)
				{
					p_queue_used_to_finalize = p_queue;
				}
			}

			p_queue_used_to_finalize->FinalizeGeneralRendering(all_waiting_queue_sems, cur_frame, frame_number_semaphore);


		}








		void GeneratePassNodeDepen()
		{
			//DEPENDENCY GENERATION
			for (const auto& pass_node : pass_nodes)
			{
				//����ÿһ��inlet��������е���Դ���ж�����д
				//case 0:����Ƕ������Դ�����˶�ȡ����Ҫȥ����Ѱ��ǰһ��д�����Դ��pass
				//case 1:����Ƕ������Դ������д�룬��Ҫȥ����Ѱ��ǰһ����ȡ����Դ��pass����Ϊ��д��ᱩ������֮ǰpass��д������ݣ�
				//TODO: ���߳��Ż�
				GeneratePassNodeDepen(*pass_node);
				std::remove(pass_node->passes_depen_set.begin(), pass_node->passes_depen_set.end(), nullptr);
			}



			//����ÿ��pass֮�����ִ�е�pass �� ÿ��pass֮ǰ������ִ�е�pass
			for (const auto& pass_node : pass_nodes)
			{
				for (const auto& p_pass_before_cur_pass : pass_node->passes_depen_set)
				{
					p_pass_before_cur_pass->passes_depen_on_cur_pass_set.insert(pass_node.get());
				}
			}
		}








		void DependencyProducedFromBufInlet(GraphicsPassNode& pass_node)
		{
			for (auto& [_, inlet] : pass_node.ins_buf)
			{
				//�ڵ�ǰpass�е�ʹ�÷�ʽusage
				const BufUsage& buf_usage = inlet.GetRsrcUsage();
				//���ʹ���˶��ķ�ʽ����
				if (Anni::RsrcAccessTypeRG::Read == buf_usage.GetRsrcAccessType())
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualBufRsrcAndUsage>>::iterator previous_outlet_itr;
					auto providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

					while (true)
					{
						//һֱ��ǰ�ң�ֱ���Ҳ����ṩ�Ľӿ�
						//providing_outlet_itorһ����outlet
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_outlet_itr = providing_itr;

							const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage().GetRsrcAccessType();

							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								pass_node.passes_depen_set.insert(providing_pass);
								break;
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
						}
						else   //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��ģ���δ���������һ֡��������һ֡����Դ������TAA������� ���� ��Դ���ڵ�ǰpass�����ģ�����
						{
							//established��Դ�ڶ�Ӧ�Ĵ���pass��һ���ᱻ���Ϊд������Ǳ���ȡ����������һ��loop���Ѿ��������passes_depen_set����������Ĵ����Ƕ����
							//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
							//{
								//	gfx_node->passes_depen_set.insert(previous_pass);
							//}
							break;
						}
					}
				}
				else if (buf_usage.GetRsrcAccessType() == Anni::RsrcAccessTypeRG::Write)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualBufRsrcAndUsage>>::iterator previous_out_itr;
					auto providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

					while (true)
					{
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_out_itr = providing_itr;


							const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage().GetRsrcAccessType();

							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								pass_node.passes_depen_set.insert(providing_pass);
								break;
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
						}
						else        //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��� ���� ��Դ���ڵ�ǰpass�����ġ�
						{
							if (inlet.GetUnderlyingRsrcItr()->second.rsrc_type == VRsrcType::Established)
							{
								//����Դ�ǵ��������£��������pass���ϵ�pass����д�룬��������Ҫ�ȵ�һ��pass��������Դ��
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

		void DependencyProducedFromBufOutlet(GraphicsPassNode& pass_node)
		{
			for (auto& [_, outlet] : pass_node.outs_buf)
			{
				//�ڵ�ǰpass�е�ʹ�÷�ʽusage
				const BufUsage& buf_usage = outlet.GetRsrcUsage();

				//���ʹ���˶��ķ�ʽ����
				if (buf_usage.GetRsrcAccessType() == Anni::RsrcAccessTypeRG::Read)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualBufRsrcAndUsage>>::iterator previous_out_itr;
					auto providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

					while (true)
					{
						//providing_outlet_itorһ����outlet
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_out_itr = providing_itr;

							const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage().GetRsrcAccessType();
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								pass_node.passes_depen_set.insert(providing_pass);
								break;
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
						}
						else        //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��ģ���δ���������һ֡��������һ֡����Դ������TAA������� ���� ��Դ���ڵ�ǰpass�����ģ�����
						{
							//established��Դ�ڶ�Ӧ�Ĵ���pass��һ���ᱻд������Ǳ���ȡ����������һ��loop���Ѿ��������passes_depen_set
							//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
							//{
							//	gfx_node->passes_depen_set.insert(previous_pass);
							//}

							break;
						}
					}
				}
				else if (buf_usage.GetRsrcAccessType() == Anni::RsrcAccessTypeRG::Write)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualBufRsrcAndUsage>>::iterator previous_out_itr;
					auto providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

					while (true)
					{
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_out_itr = providing_itr;

							const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage().GetRsrcAccessType();

							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								pass_node.passes_depen_set.insert(providing_pass);
								break;
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
						}
						else        //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��� ���� ��Դ���ڵ�ǰpass�����ġ�
						{
							if (outlet.GetUnderlyingRsrcItr()->second.rsrc_type == VRsrcType::Established)
							{
								//����Դ�ǵ��������£��������pass���ϵ�pass����д�룬��������Ҫ�ȵ�һ��pass��������Դ��
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


		void DependencyProducedFromTexInlet(GraphicsPassNode& pass_node)
		{
			for (auto& [_, inlet] : pass_node.ins_tex)
			{
				//�ڵ�ǰpass�е�ʹ�÷�ʽusage
				std::variant<TexUsage, AttachUsage>& curlet_usage = inlet.GetRsrcUsage();
				Anni::RsrcAccessTypeRG asscess_type
					= std::visit([](const auto& variant_usage) -> Anni::RsrcAccessTypeRG
						{
							return variant_usage.access_type;
						}, curlet_usage);


				//���ʹ���˶��ķ�ʽ����
				if (asscess_type == Anni::RsrcAccessTypeRG::Read)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualTexRsrcAndUsage>>::iterator previous_outlet_itr;
					auto providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();
					Anni::ImgSyncInfo previous_pass_syn_info = std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
						{
							return variant_usage.GetSynInfo();
						}, curlet_usage);

					while (true)
					{
						//һֱ��ǰ�ң�ֱ���Ҳ����ṩ�Ľӿ�
						//providing_outlet_itorһ����outlet
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;
							auto providing_syn_info = std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
								{
									return variant_usage.GetSynInfo();
								}, providing_itr->second.GetRsrcUsage());


							previous_pass = providing_pass;
							previous_outlet_itr = providing_itr;

							Anni::RsrcAccessTypeRG& cur_asscess_type
								= std::visit([](auto& variant_usage) -> Anni::RsrcAccessTypeRG&
									{
										return variant_usage.access_type;
									}, providing_itr->second.GetRsrcUsage());

							const auto& providing_pass_access_type = cur_asscess_type;

							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								pass_node.passes_depen_set.insert(providing_pass);
								break;
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								//����֮ǰ��syn info���Աȣ����img layout��ͬ����ô��Ҫ����д�뿴����
								if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
								{
									pass_node.passes_depen_set.insert(providing_pass);
									break;
								}
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
							previous_pass_syn_info = providing_syn_info;
						}
						else   //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��ģ���δ���������һ֡��������һ֡����Դ������TAA������� ���� ��Դ���ڵ�ǰpass�����ģ�����
						{
							//established��Դ�ڶ�Ӧ�Ĵ���pass��һ���ᱻ���Ϊд������Ǳ���ȡ����������һ��loop���Ѿ��������passes_depen_set����������Ĵ����Ƕ����
							//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
							//{
								//	gfx_node->passes_depen_set.insert(previous_pass);
							//}
							break;
						}
					}
				}
				else if (asscess_type == Anni::RsrcAccessTypeRG::Write)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualTexRsrcAndUsage>>::iterator previous_out_itr;
					auto providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

					Anni::ImgSyncInfo previous_pass_syn_info = std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
						{
							return variant_usage.GetSynInfo();
						}, curlet_usage);


					while (true)
					{
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_out_itr = providing_itr;

							auto providing_syn_info = std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
								{
									return variant_usage.GetSynInfo();
								}, providing_itr->second.GetRsrcUsage());


							Anni::RsrcAccessTypeRG& cur_asscess_type
								= std::visit([&](auto& variant_usage) -> Anni::RsrcAccessTypeRG&
									{
										return variant_usage.access_type;
									}, providing_itr->second.GetRsrcUsage());
							const auto providing_pass_access_type = cur_asscess_type;

							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								//����֮ǰ��syn info���Աȣ����img layout��ͬ����ô��Ҫ����д�뿴����
								if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
								{
									providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
								}
								else
								{
									pass_node.passes_depen_set.insert(providing_pass);
									break;
								}
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}

							previous_pass_syn_info = providing_syn_info;
						}
						else        //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��� ���� ��Դ���ڵ�ǰpass�����ġ�
						{
							if (inlet.GetUnderlyingRsrcItr()->second.rsrc_type == VRsrcType::Established)
							{
								//����Դ�ǵ��������£��������pass���ϵ�pass����д�룬��������Ҫ�ȵ�һ��pass��������Դ��
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



		void DependencyProducedFromTexOutlet(GraphicsPassNode& pass_node)
		{
			for (auto& [_, outlet] : pass_node.outs_tex)
			{
				//�ڵ�ǰpass�е�ʹ�÷�ʽusage
				std::variant<TexUsage, AttachUsage>& curlet_usage = outlet.GetRsrcUsage();

				Anni::RsrcAccessTypeRG asscess_type
					= std::visit([&](const auto& variant_usage) -> Anni::RsrcAccessTypeRG
						{
							return variant_usage.access_type;
						}, curlet_usage);


				//���ʹ���˶��ķ�ʽ����
				if (Anni::RsrcAccessTypeRG::Read == asscess_type)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualTexRsrcAndUsage>>::iterator previous_out_itr;
					auto providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

					Anni::ImgSyncInfo previous_pass_syn_info =
						std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, curlet_usage);



					while (true)
					{
						//providing_outlet_itorһ����outlet
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_out_itr = providing_itr;

							auto providing_syn_info = std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
								{
									return variant_usage.GetSynInfo();
								}, providing_itr->second.GetRsrcUsage());

							Anni::RsrcAccessTypeRG cur_asscess_type
								= std::visit([&](const auto& variant_usage) -> Anni::RsrcAccessTypeRG
									{
										return variant_usage.access_type;
									}, providing_itr->second.GetRsrcUsage());


							const auto providing_pass_access_type = cur_asscess_type;
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								pass_node.passes_depen_set.insert(providing_pass);
								break;
							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								//����֮ǰ��syn info���Աȣ����img layout��ͬ����ô��Ҫ����д�뿴����
								if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
								{
									pass_node.passes_depen_set.insert(providing_pass);
									break;
								}
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
							previous_pass_syn_info = providing_syn_info;
						}
						else        //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��ģ���δ���������һ֡��������һ֡����Դ������TAA������� ���� ��Դ���ڵ�ǰpass�����ģ�����
						{
							//established��Դ�ڶ�Ӧ�Ĵ���pass��һ���ᱻд������Ǳ���ȡ����������һ��loop���Ѿ��������passes_depen_set
							//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
							//{
							//	gfx_node->passes_depen_set.insert(previous_pass);
							//}

							break;
						}
					}
				}
				else if (Anni::RsrcAccessTypeRG::Write == asscess_type)
				{
					GraphicsPassNode* previous_pass = &pass_node;
					std::unordered_map<std::string, RsrcOutlet<VirtualTexRsrcAndUsage>>::iterator previous_out_itr;
					auto providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

					Anni::ImgSyncInfo previous_pass_syn_info =
						std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
							{
								return variant_usage.GetSynInfo();
							}, curlet_usage);



					while (true)
					{
						if (providing_pass_and_source_outlet_itr)
						{
							auto providing_itr = providing_pass_and_source_outlet_itr->second;
							auto providing_pass = providing_pass_and_source_outlet_itr->first;

							previous_pass = providing_pass;
							previous_out_itr = providing_itr;

							auto providing_syn_info = std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
								{
									return variant_usage.GetSynInfo();
								}, providing_itr->second.GetRsrcUsage());


							Anni::RsrcAccessTypeRG cur_asscess_type
								= std::visit([&](const auto& variant_usage) -> Anni::RsrcAccessTypeRG
									{
										return variant_usage.access_type;
									}, providing_itr->second.GetRsrcUsage());

							const auto providing_pass_access_type = cur_asscess_type;

							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Read)
							{
								//����֮ǰ��syn info���Աȣ����img layout��ͬ����ô��Ҫ����д�뿴����
								if (previous_pass_syn_info.layout_inpass != providing_syn_info.layout_inpass)
								{
									providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
								}
								else
								{
									pass_node.passes_depen_set.insert(providing_pass);
									break;
								}


							}
							if (providing_pass_access_type == Anni::RsrcAccessTypeRG::Write)
							{
								providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
							}
						}
						else        //��������ṩ��Դ��pass�����ڵ��������ô˵����Դһ��ʼ������rendergraph֮��� ���� ��Դ���ڵ�ǰpass�����ġ�
						{
							if (outlet.GetUnderlyingRsrcItr()->second.rsrc_type == VRsrcType::Established)
							{
								//����Դ�ǵ��������£��������pass���ϵ�pass����д�룬��������Ҫ�ȵ�һ��pass��������Դ��
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

		void GeneratePassNodeDepen(GraphicsPassNode& pass_node)
		{
			DependencyProducedFromBufInlet(pass_node);
			DependencyProducedFromBufOutlet(pass_node);
			DependencyProducedFromTexInlet(pass_node);
			DependencyProducedFromTexOutlet(pass_node);
		}



		size_t FindIndexInPassNodeArray(GraphicsPassNode* node_pass, const std::vector<std::unique_ptr<GraphicsPassNode>>& all_nodes)
		{
			for (auto gfx_node_itr = all_nodes.begin(); gfx_node_itr < all_nodes.end(); ++gfx_node_itr)
			{
				if (gfx_node_itr->get() == node_pass)
				{
					return std::distance(all_nodes.begin(), gfx_node_itr);
				}
			}
			assert(!"no pass found!");
		}


		void VisitNode(size_t cur_index, GraphicsPassNode* cur_node_pass, std::vector<bool>& visited, std::vector<bool>& on_stack, std::vector<GraphicsPassNode*>& topologically_sorted_nodes)
		{
			if (visited[cur_index] && on_stack[cur_index])
			{
				assert(!"found a circular dependency and must abort.");
			}
			if (visited[cur_index])
			{
				return;
			}

			visited[cur_index] = true;
			on_stack[cur_index] = true;

			for (GraphicsPassNode* adj_list : cur_node_pass->passes_depen_on_cur_pass_set)
			{
				VisitNode(FindIndexInPassNodeArray(adj_list, pass_nodes), adj_list, visited, on_stack, topologically_sorted_nodes);
			}

			topologically_sorted_nodes.push_back(cur_node_pass);
			on_stack[cur_index] = false;
		}


		void TopologicalDFSSort()
		{
			//����������dfs�㷨�����ֲ��
			std::vector<bool> visitedNodes(pass_nodes.size(), false);
			std::vector<bool> onStackNodes(pass_nodes.size(), false);

			for (auto gfx_node_itr = pass_nodes.begin(); gfx_node_itr < pass_nodes.end(); ++gfx_node_itr)
			{
				auto index = std::distance(pass_nodes.begin(), gfx_node_itr);

				VisitNode(index, gfx_node_itr->get(), visitedNodes, onStackNodes, topologically_sorted_nodes);

				if (topologically_sorted_nodes.size() == pass_nodes.size())
				{
					break;
				}
			}
			std::ranges::reverse(topologically_sorted_nodes);
		}



		void DependencyLevelsGeneration()
		{
			///************************************************************
			//DEPENDENCY LEVELS(LAYERS) GENERATION��λ���:
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





		Anni::Queue& GetSuitableQueue(GraphicsPassNode& pass)
		{

			PassType  pass_type = pass.GetRenderpassType();
			QueueCapability queue_cap_requirement;

			if (pass_type & VkRenderpassBaseRG::Type::Graphics)
			{
				queue_cap_requirement.graphics = true;
			}

			if (pass_type & VkRenderpassBaseRG::Type::Compute)
			{
				queue_cap_requirement.compute = true;
			}

			if (pass_type & VkRenderpassBaseRG::Type::Transfor)
			{
				queue_cap_requirement.transfer = true;
			}

			if (pass_type & VkRenderpassBaseRG::Type::Present)
			{
				queue_cap_requirement.present = true;
			}
			auto& target_queue = queue_manager.GetSuitableQueue(queue_cap_requirement);
			return target_queue;
		}


		void GPUWorkDistribution()
		{

			///************************************************************
			//DISTRIBUTE WORK TO MULTIPLE QUEUES ONTO GPU:
			///************************************************************
			//std::unordered_map<DeviceManager::VkExecutionQueue *, std::list<PassNode *>>         queue_to_passes_on_it;
			//std::unordered_map<PassNode *, std::pair<DeviceManager::VkExecutionQueue *, size_t>> pass_to_queue_info;

			queue_manager.QueuesNumDisbatchedPassesToZero();

			size_t monotonical_assigned_index = 1;
			for (auto level = 0; level <= max_level; ++level)
			{
				for (auto& ptr_pass : topologically_sorted_nodes)
				{
					if (ptr_pass->cur_level == level)
					{

						auto& target_queue = GetSuitableQueue(*ptr_pass);
						auto  ptr_queue = &target_queue;


						if (queue_to_passes_on_it.contains(ptr_queue))
						{
							queue_to_passes_on_it[ptr_queue].push_back(ptr_pass);
						}
						else
						{
							queue_to_passes_on_it.emplace(ptr_queue, std::vector<GraphicsPassNode*>{ptr_pass});
						}

						auto result_itr = pass_to_queue_info.emplace(ptr_queue, std::pair<Queue*, size_t>{ptr_queue, monotonical_assigned_index});

						VULKAN_HPP_ASSERT(result_itr.second, "the pass has been distributed to queue!");
						monotonical_assigned_index++;
					}
				}
			}

			std::vector<Queue*> all_queue_in_use;        //����ȷ��һ��queue��˳�򣬷���֮����index����
			// Extract keys from the map and store them in the vector
			std::ranges::transform(queue_to_passes_on_it, std::back_inserter(all_queue_in_use),
				[](const std::pair<Queue*, std::vector<GraphicsPassNode*>>& pair)
				{
					return pair.first;        // Extract the key
				});

		}

		void SyncPrimitivesAquisition()
		{
			for (auto itr = topologically_sorted_nodes.begin(); std::next(itr) != topologically_sorted_nodes.end(); ++itr)
			{
				diff_queue_sync_sema.emplace(PassNodePair{ *itr, *std::next(itr) }, semaphore_pool.AcquireTimelineSem());
			}
		}



		void PartitionPassesAtTheSameLevel()
		{
			///************************************************************
			//ͬlevel������passes����:
			///************************************************************

			//���ȶ������е���Դ������ʹ�õ���Դ��pass����layer�����Ժ�ÿһ��layer�㼶�ڵ�pass����Ҫ�������򣬷������ִ��˳�򶨲�����������д����ͬ��
			for (auto& [_, vrsrc] : rg_buffers_map)
			{
				for (auto level = 0; level <= max_level; ++level)
				{
					vrsrc.level_to_passes_attached_to.emplace(level, std::vector<GraphicsPassNode*>{});
					vrsrc.level_to_passes_attached_to_partitioned.emplace(level, std::vector<std::vector<GraphicsPassNode*>>{});
				}

				//ͬʱ�����ڵ�ǰͬһ��layer������pass������Ҫ��һ��ִ��˳��
				std::ranges::for_each(
					vrsrc.passes_access_this_rsrc, [&vrsrc](const std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>& pass_assess_pair)
					{
						auto cur_pass = pass_assess_pair.first;
						vrsrc.level_to_passes_attached_to.at(cur_pass->cur_level).push_back(cur_pass);
					});


				//�������Щͬlayer��passes��������ִ�е�queue fam���зֿ�
				//queue fam�ֿ��ڻ�Ҫ���� queue �ķֿ�
				for (auto& [level, passes_at_same_levels] : vrsrc.level_to_passes_attached_to)
				{
					//for (auto& pass : passes_at_same_levels)
					//{
					//	vrsrc.level_to_unique_queues.at(level).insert(pass_to_queue_info.at(pass).first);
					//}

					std::set<size_t> level_to_unique_queue_fam_index; //������set�ܹؼ���
					for (auto& pass : passes_at_same_levels)
					{
						level_to_unique_queue_fam_index.insert(pass_to_queue_info.at(pass).first->GetQueueCap().queue_family_index);
					}

					std::vector<std::vector<GraphicsPassNode*>> partitions_by_queue_fam_then_by_queue;

					std::vector<GraphicsPassNode*> partition_result;
					for (auto unique_queue_fam : level_to_unique_queue_fam_index)        //unique queueΪָ��
					{
						//����queue family��partition
						std::vector<GraphicsPassNode*> queue_fam_partition;
						std::ranges::copy_if(passes_at_same_levels, std::back_inserter(queue_fam_partition),
							[this, unique_queue_fam](GraphicsPassNode* const pass) -> bool
							{
								return (this->pass_to_queue_info.at(pass).first->GetQueueCap().queue_family_index == unique_queue_fam);
							});
						//����ִ�к�queue_fam_partition�����е�pass��������ͬһ��queue family��

						std::set<Queue*> unique_queues_on_the_same_queue_fam;      //����Ҫ��std::set
						std::ranges::for_each
						(
							queue_fam_partition,
							[this, &unique_queues_on_the_same_queue_fam](GraphicsPassNode* pass)
							{
								unique_queues_on_the_same_queue_fam.insert(this->pass_to_queue_info.at(pass).first);

							}
						);
						//������queue����partition��
						for (auto unique_queue : unique_queues_on_the_same_queue_fam)        //unique queueΪָ��
						{
							std::set<GraphicsPassNode*> partition;
							std::ranges::copy_if(queue_fam_partition, std::back_inserter(partition),
								[this, unique_queue](GraphicsPassNode* pass) -> bool
								{
									return this->pass_to_queue_info.at(pass).first == unique_queue;
								});
							partition_result.insert(partition_result.end(), partition.begin(), partition.end());
							partitions_by_queue_fam_then_by_queue.push_back(std::vector<GraphicsPassNode*>{ partition.begin(), partition.end() });
						}
					}
					//pass0 Queue0 | pass1 Queue0 | pass2 Queue0 | pass3 Queue1 | pass4 Queue1 | pass5 Queue1 ���ﱣ֤�˲�ͬqueue�������һ��
					passes_at_same_levels = std::move(partition_result);
					vrsrc.level_to_passes_attached_to_partitioned.insert_or_assign(level, std::move(partitions_by_queue_fam_then_by_queue));
				}
			}


			//���ȶ������е���Դ������ʹ�õ���Դ��pass����layer�����Ժ�ÿһ��layer�㼶�ڵ�pass����Ҫ�������򣬷������ִ��˳�򶨲�����������д����ͬ��
			for (auto& [_, vrsrc] : rg_textures_map)
			{
				for (auto level = 0; level <= max_level; ++level)
				{
					vrsrc.level_to_passes_attached_to.emplace(level, std::vector<GraphicsPassNode*>{});
					vrsrc.level_to_passes_attached_to_partitioned.emplace(level, std::vector<std::vector<GraphicsPassNode*>>{});
					//vrsrc.level_to_t_semas.emplace(level,std::make_shared<TimelineSemWrapper>( device_manager) );
				}

				//ͬʱ�����ڵ�ǰͬһ��layer������pass������Ҫ��һ��ִ��˳��
				std::ranges::for_each(
					vrsrc.passes_access_this_rsrc, [&vrsrc](const std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>& pass_assess_pair)
					{
						auto cur_pass = pass_assess_pair.first;
						vrsrc.level_to_passes_attached_to.at(cur_pass->cur_level).push_back(cur_pass);
					});


				//�������Щͬlayer��passes��������ִ�е�queue fam���зֿ�
				//queue fam�ֿ��ڻ�Ҫ���� queue �ķֿ�
				for (auto& [level, passes_at_same_levels] : vrsrc.level_to_passes_attached_to)
				{
					//for (auto& pass : passes_at_same_levels)
					//{
					//	vrsrc.level_to_unique_queues.at(level).insert(pass_to_queue_info.at(pass).first);
					//}

					std::set<size_t> level_to_unique_queue_fam_index;
					for (auto& pass : passes_at_same_levels)
					{
						level_to_unique_queue_fam_index.insert(pass_to_queue_info.at(pass).first->GetQueueCap().queue_family_index);
					}

					std::vector<std::vector<GraphicsPassNode*>> partitions_by_queue_fam_then_by_queue;
					std::vector<GraphicsPassNode*> partition_result;
					for (auto unique_queue_fam : level_to_unique_queue_fam_index)        //unique queueΪָ��
					{
						//����queue family��partition
						std::vector<GraphicsPassNode*> queue_fam_partition;
						std::ranges::copy_if(passes_at_same_levels, std::back_inserter(queue_fam_partition),
							[this, unique_queue_fam](GraphicsPassNode* const pass) -> bool
							{
								return (this->pass_to_queue_info.at(pass).first->GetQueueCap().queue_family_index == unique_queue_fam);
							});
						//����ִ�к�queue_fam_partition�����е�pass��������ͬһ��queue family��

						std::unordered_set<Queue*> unique_queues_on_the_same_queue_fam;
						std::ranges::for_each
						(
							queue_fam_partition,
							[this, &unique_queues_on_the_same_queue_fam](GraphicsPassNode* pass)
							{
								unique_queues_on_the_same_queue_fam.insert(this->pass_to_queue_info.at(pass).first);

							}
						);
						//������queue����partition��
						for (auto unique_queue : unique_queues_on_the_same_queue_fam)        //unique queueΪָ��
						{
							std::set<GraphicsPassNode*> partition;
							std::ranges::copy_if(queue_fam_partition, std::back_inserter(partition),
								[this, unique_queue](GraphicsPassNode* pass) -> bool
								{
									return this->pass_to_queue_info.at(pass).first == unique_queue;
								});

							partition_result.insert(partition_result.end(), partition.begin(), partition.end());
							partitions_by_queue_fam_then_by_queue.push_back(std::vector<GraphicsPassNode*>{ partition.begin(), partition.end() });
						}
					}
					//pass0 Queue0 | pass1 Queue0 | pass2 Queue0 | pass3 Queue1 | pass4 Queue1 | pass5 Queue1 ���ﱣ֤�˲�ͬqueue�������һ��
					passes_at_same_levels = std::move(partition_result);
					vrsrc.level_to_passes_attached_to_partitioned.insert_or_assign(level, std::move(partitions_by_queue_fam_then_by_queue));
				}
			}

		}


		void MarkLayersMultiAccessSameRsrc()
		{
			for (size_t i = 0; i <= max_level; ++i)
			{
				if_level_accesses_same_rsrc_multimes.insert_or_assign(i, false);
			}

			for (auto& [_, vrsrc] : rg_buffers_map)
			{
				for (auto& [level, passes_at_the_same_level] : vrsrc.level_to_passes_attached_to)
				{
					if (passes_at_the_same_level.size() > 1)
					{
						if_level_accesses_same_rsrc_multimes[level] = true;
					}
				}
			}

			for (auto& [_, vrsrc] : rg_textures_map)
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

		bool SortByTopologicallySortedPasses(const GraphicsPassNode* a, const GraphicsPassNode* b, const std::vector<GraphicsPassNode*>& order)
		{
			auto it_a = std::find(order.begin(), order.end(), a);
			auto it_b = std::find(order.begin(), order.end(), b);

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
			else if (it_b != order.end())
			{
				return false;
			}
			// If neither element is found, maintain the original order
			return a < b;
		}



		void SortPassesAccessingRsrc()
		{
			//��ÿ����Դ��ʹ���ߣ�passes����������
			for (auto& [_, vrsc] : rg_buffers_map)
			{
				// Sorting the unordered vector based on the order of the order vector(that is, topologically_sorted_nodes)
				std::sort(vrsc.passes_access_this_rsrc.begin(), vrsc.passes_access_this_rsrc.end(),
					[&](const std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>& pair_left, const std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>& pair_right)
					{
						return SortByTopologicallySortedPasses(pair_left.first, pair_right.first, topologically_sorted_nodes);
					});
			}

			for (const auto& [_, vrsc] : rg_textures_map)
			{
				// Sorting the unordered vector based on the order of the order vector
				std::sort(vrsc.passes_access_this_rsrc.begin(), vrsc.passes_access_this_rsrc.end(),
					[&](const std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>& pair_left, const std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>& pair_right)
					{
						return SortByTopologicallySortedPasses(pair_left.first, pair_right.first, topologically_sorted_nodes);
					});
			}
		}


		void SyncPrimitivesInsertionInletBuffer(GraphicsPassNode* const cur_pass)
		{
			auto cur_pass_level = cur_pass->cur_level;
			for (auto& [_, cur_let] : cur_pass->ins_buf)
			{
				VirtualBuffer& curlet_underlying_rsrc = cur_let.GetUnderlyingRsrcItr()->second;
				BufUsage& curlet_usage = cur_let.GetRsrcUsage();
				const IRsrcUsage::RsrcOrigin curlet_origin = curlet_usage.GetRsrcOrigin();

				//ֱ���ڵ�ǰpass��������Դ��ͬ��ԭ����룺
				if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
				{
					auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
					cur_pass->InsertSyncInfoForInitalUsage(
						source_sync,
						curlet_usage.GetSynInfo(),
						cur_let.GetUnderlyingRsrcItr()
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
							cur_let.GetUnderlyingRsrcItr()
						);
					}
				}


				//�õ�����ʹ�ø���Դ��pass������֮ǰ��ÿһ��layer������ͬ��ԭ����롣���統ǰlayer��5������������Ҫ�ҵ�layer=0,1,2,3,4
				for (const auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.level_to_passes_attached_to_partitioned)
				{
					//���ǰ���layer i �����˶�ĳһ��Դͬʱaccess�������
					//layer i ���ܺ��ж��ִ���ڲ�ͬqueue�ϵ�passes�����������ǰ��շֿ�����ģ���������£�����ֻ��Ҫ�����һ���е�����pass�͵�ǰpass֮�����ͬ������
					if (level < cur_pass_level)
					{
						const auto& last_passes_group = passes_at_same_level_partitioned.back();
						const auto  last_queue = pass_to_queue_info.at(last_passes_group.back());
						const auto  queue_which_cur_pass_execute = pass_to_queue_info.at(cur_pass);

						if (last_queue == queue_which_cur_pass_execute)
						{
							auto last_pass_executing_on_last_queue_group = last_passes_group.back();
							auto& target_let = cur_let;
							auto underlying_buf = target_let.GetUnderlyingRsrcItr();

							auto sor_pass = last_pass_executing_on_last_queue_group;
							auto tar_pass = cur_pass;

							auto founded_buf_usage = std::ranges::find_if(sor_pass->buf_usages,
								[&curlet_underlying_rsrc](const auto& buf_usage)
								{
									return (&(buf_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							VULKAN_HPP_ASSERT(founded_buf_usage != sor_pass->buf_usages.end());

							BufUsage& sor_buf_usage = founded_buf_usage->usage;
							BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

							cur_pass->InsertSameQueueSyncInfo(
								sor_pass,
								tar_pass,
								queue_which_cur_pass_execute.first,
								sor_buf_usage.GetSynInfo(),
								tar_buf_usage.GetSynInfo(),
								underlying_buf
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
							const auto underlying_buf = target_let.GetUnderlyingRsrcItr();

							auto founded_buf_usage = std::ranges::find_if(sor_pass->buf_usages,
								[&curlet_underlying_rsrc](const auto& buf_usage)
								{
									return (&(buf_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_buf_usage != sor_pass->buf_usages.end());

							BufUsage& sor_buf_usage = founded_buf_usage->usage;
							BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

							const Anni::SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
							const Anni::WaitValue   tar_wait_val{ sync_sema->GetLastValue() + 1 };

							Anni::SemInsertInfoSafe sor_insert_info(
								sor_signal_val,
								sor_buf_usage.sync_info.stage_mask
							);

							Anni::SemInsertInfoSafe tar_insert_info{
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
								target_let.GetUnderlyingRsrcItr(),
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
								target_let.GetUnderlyingRsrcItr(),
								sor_insert_info,
								tar_insert_info
							);
						}
					}


					//ͬʱ�����ڵ�ǰͬһ��layer������pass������Ҫ��һ��ִ��˳��
					if (level == cur_pass_level)
					{
						//passes_at_same_levels�Ų�һ���� ����queue �ֿ��
						// ����passes_at_same_levels��   pass0 , pass1 , pass4, pass5 , pass7 , pass9
						// ��0  queue3 ��|pass0��pass1��pass4| ;  ��1  queue 2 ��|pass5��pass7��pass9|

						//��ͬһ��level�о������ͬ������ͬһlevel�е�passӦ��ֻ�����ͬʱ��һ����Դ��������������ֻ��Ҫ������ͬqueue��queue ownership transfer������?
						//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);

						auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];

						std::vector<Queue*> unique_queues_ordered;
						std::ranges::transform(passes_at_same_level_partitioned, std::back_inserter(unique_queues_ordered),
							[this](const std::vector<GraphicsPassNode*>& inner_pass_group)->Queue*
							{
								return  pass_to_queue_info.at(inner_pass_group.front()).first;
							}
						);


						//auto  cur_pass_pos_in_cur_level = std::ranges::find(passes_at_same_levels.begin(), passes_at_same_levels.end(), cur_pass);
						auto  cur_pass_pos_in_cur_level = std::ranges::find(passes_at_same_levels, cur_pass);
						auto  cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);

						VULKAN_HPP_ASSERT(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
						VULKAN_HPP_ASSERT(cur_pass_pos_in_cur_level != passes_at_same_levels.end(), "Current pass is not found at the level!");

						auto sor_pass = cur_pass;
						auto tar_pass = cur_pass;

						//�����ǰpass�������level�е�һ��pass
						if (cur_pass_pos_in_cur_level != passes_at_same_levels.begin())
						{
							// Get the iterator to the previous element
							auto previous_pass_itr = std::prev(cur_pass_pos_in_cur_level);
							// Access the previous element
							sor_pass = *previous_pass_itr;
							tar_pass = cur_pass;

							const auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
							const auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;

							auto founded_buf_usage = std::ranges::find_if(sor_pass->buf_usages,
								[&curlet_underlying_rsrc](const auto& buf_usage)
								{
									return (&(buf_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_buf_usage != sor_pass->buf_usages.end());


							auto& target_let = cur_let;
							BufUsage& sor_buf_usage = founded_buf_usage->usage;
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

							const Anni::SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
							const Anni::WaitValue   tar_wait_val{ sync_sema->GetLastValue() + 1 };

							Anni::SemInsertInfoSafe sor_insert_info(
								sor_signal_val,
								sor_buf_usage.sync_info.stage_mask
							);

							Anni::SemInsertInfoSafe tar_insert_info{
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
									target_let.GetUnderlyingRsrcItr()
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
									target_let.GetUnderlyingRsrcItr(),
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
									target_let.GetUnderlyingRsrcItr(),
									sor_insert_info,
									tar_insert_info
								);
							}

						}
					}
				}
			}
		}

		void SyncPrimitivesInsertionOutletBuffer(GraphicsPassNode* const cur_pass)
		{
			auto cur_level = cur_pass->cur_level;
			for (auto& [_, cur_let] : cur_pass->outs_buf)
			{
				VirtualBuffer& curlet_underlying_rsrc = cur_let.GetUnderlyingRsrcItr()->second;
				BufUsage& curlet_usage = cur_let.GetRsrcUsage();
				const IRsrcUsage::RsrcOrigin curlet_origin = curlet_usage.GetRsrcOrigin();

				//ֱ���ڵ�ǰpass��������Դ��ͬ��ԭ����룺
				if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
				{
					auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
					cur_pass->InsertSyncInfoForInitalUsage(
						source_sync,
						curlet_usage.GetSynInfo(),
						cur_let.GetUnderlyingRsrcItr()
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
							cur_let.GetUnderlyingRsrcItr());
					}

				}

				//�õ�����ʹ�ø���Դ��pass������֮ǰ��ÿһ��layer������ͬ��ԭ����롣���統ǰlayer��5������������Ҫ�ҵ�layer=0,1,2,3,4
				for (const auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.level_to_passes_attached_to_partitioned)
				{
					//���ǰ���layer i �����˶�ĳһ��Դͬʱaccess�������
					//layer i ���ܺ��ж��ִ���ڲ�ͬqueue�ϵ�passes�����������ǰ��շֿ�����ģ���������£�����ֻ��Ҫ�����һ���е�����pass�͵�ǰpass֮�����ͬ������
					if (level < cur_level)
					{
						const auto& last_passes_group = passes_at_same_level_partitioned.back();
						const auto  last_queue = pass_to_queue_info.at(last_passes_group.back());
						const auto  queue_which_cur_pass_execute = pass_to_queue_info.at(cur_pass);

						if (last_queue == queue_which_cur_pass_execute)
						{
							auto last_pass_executing_on_last_queue_group = last_passes_group.back();
							auto& target_let = cur_let;
							auto underlying_buf = target_let.GetUnderlyingRsrcItr();

							auto sor_pass = last_pass_executing_on_last_queue_group;
							auto tar_pass = cur_pass;

							auto founded_buf_usage = std::ranges::find_if(sor_pass->buf_usages,
								[&curlet_underlying_rsrc](const auto& buf_usage)
								{
									return (&(buf_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_buf_usage != sor_pass->buf_usages.end());

							BufUsage& sor_buf_usage = founded_buf_usage->usage;
							BufUsage& tar_buf_usage = target_let.GetRsrcUsage();

							cur_pass->InsertSameQueueSyncInfo(
								sor_pass,
								tar_pass,
								queue_which_cur_pass_execute.first,
								sor_buf_usage.GetSynInfo(),
								tar_buf_usage.GetSynInfo(),
								underlying_buf
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
							const auto underlying_buf = target_let.GetUnderlyingRsrcItr();

							auto founded_buf_usage = std::ranges::find_if(sor_pass->buf_usages,
								[&curlet_underlying_rsrc](const auto& buf_usage)
								{
									return (&(buf_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_buf_usage != sor_pass->buf_usages.end());

							BufUsage& sor_buf_usage = founded_buf_usage->usage;
							BufUsage& tar_buf_usage = target_let.GetRsrcUsage();


							const Anni::SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
							const Anni::WaitValue   tar_wait_val{ sync_sema->GetLastValue() + 1 };

							Anni::SemInsertInfoSafe sor_insert_info(
								sor_signal_val,
								sor_buf_usage.sync_info.stage_mask
							);

							Anni::SemInsertInfoSafe tar_insert_info{
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
								target_let.GetUnderlyingRsrcItr(),
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
								target_let.GetUnderlyingRsrcItr(),
								sor_insert_info,
								tar_insert_info
							);
						}
					}
					//ͬʱ�����ڵ�ǰͬһ��layer������pass������Ҫ��һ��ִ��˳��
					if (level == cur_level)
					{
						//passes_at_same_levels�Ų�һ���� ����queue �ֿ��
						// ����passes_at_same_levels��   pass0 , pass1 , pass4, pass5 , pass7 , pass9
						// ��0  queue3 ��|pass0��pass1��pass4| ;  ��1  queue 2 ��|pass5��pass7��pass9|

						//��ͬһ��level�о������ͬ������ͬһlevel�е�passӦ��ֻ�����ͬʱ��һ����Դ��������������ֻ��Ҫ������ͬqueue��queue ownership transfer������?
						//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);
						auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];

						std::vector<Queue*> unique_queues_ordered;
						std::transform(passes_at_same_level_partitioned.begin(), passes_at_same_level_partitioned.end(), std::back_inserter(unique_queues_ordered),
							[this](const std::vector<GraphicsPassNode*>& inner_pass_group)->Queue*
							{
								return  pass_to_queue_info.at(inner_pass_group.front()).first;
							}
						);

						auto  cur_pass_pos_in_cur_level = std::find(passes_at_same_levels.begin(), passes_at_same_levels.end(), cur_pass);
						auto  cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);

						VULKAN_HPP_ASSERT(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
						VULKAN_HPP_ASSERT(cur_pass_pos_in_cur_level != passes_at_same_levels.end(), "Current pass is not found at the level!");

						auto sor_pass = cur_pass;
						auto tar_pass = cur_pass;

						//�����ǰpass�������level�е�һ��pass
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

							Anni::Queue* sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
							Anni::Queue* tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;

							auto founded_buf_usage = std::ranges::find_if(sor_pass->buf_usages,
								[&curlet_underlying_rsrc](const auto& buf_usage)
								{
									return (&(buf_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_buf_usage != sor_pass->buf_usages.end());


							auto& target_let = cur_let;
							BufUsage& tar_buf_usage = target_let.GetRsrcUsage();
							BufUsage& sor_buf_usage = founded_buf_usage->usage;


							//Source pass     ------------------------->       Cur pass
							//Sync primitive
							auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

							const Anni::SignalValue sor_signal_val{ sync_sema->GetLastValue() + 1 };
							const Anni::WaitValue   tar_wait_val{ sync_sema->GetLastValue() + 1 };

							Anni::SemInsertInfoSafe sor_insert_info(
								sor_signal_val,
								sor_buf_usage.sync_info.stage_mask
							);

							Anni::SemInsertInfoSafe tar_insert_info{
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
									target_let.GetUnderlyingRsrcItr()
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
									target_let.GetUnderlyingRsrcItr(),
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
									target_let.GetUnderlyingRsrcItr(),
									sor_insert_info,
									tar_insert_info
								);
							}
						}
					}
				}
			}
		}





		void SyncPrimitivesInsertionInletTexture(GraphicsPassNode* const cur_pass)
		{

			auto& swap_imgs_finshed_using_by_cur_pass = vec_swap_imgs_finshed_using.back();
			auto cur_level = cur_pass->cur_level;
			//����inlet tex

			for (auto& [_, cur_let] : cur_pass->ins_tex)
			{
				VirtualTexture& curlet_underlying_rsrc = cur_let.GetUnderlyingRsrcItr()->second;
				auto& curlet_usage = cur_let.GetRsrcUsage();
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

				//ֱ���ڵ�ǰpass��������Դ��ͬ��ԭ����룺
				if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
				{
					auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
					cur_pass->InsertSyncInfoForInitalUsage(
						source_sync,
						curlet_sync_info,
						cur_let.GetUnderlyingRsrcItr()
					);
				}


				//����rendergraph�ⲿ��
				if (IRsrcUsage::RsrcOrigin::FromOutSide == curlet_origin)
				{
					auto first_pass = curlet_underlying_rsrc.passes_access_this_rsrc.front().first;
					if (first_pass == cur_pass)
					{
						if (curlet_underlying_rsrc.p_rsrc)
						{
							const auto& tex_img = curlet_underlying_rsrc.p_rsrc->GetTextureImage();
							auto& sem_wrapper = tex_img->GetSemUsedToTransfer();

							if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState())//˵����û������ɣ������Ļ���������Ҫ�������semaphore��ͬ��ԭ��
							{
								auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();

								SemInsertInfoSafe sem_insert_info(Anni::WaitValue{ sem_wrapper->GetLastValue() }, curlet_sync_info.stage_mask);

								cur_pass->InsertSyncInfoForInitalLoad(
									source_sync,
									curlet_sync_info,
									cur_let.GetUnderlyingRsrcItr(),
									sem_wrapper,
									sem_insert_info
								);

								if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
								{
									{
										//maybe some previous frames is using the current image , so wait
										auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->swap_img_rdy_4_rendering_helper_sem;
										const auto wait_value = Anni::WaitValue{ rdy_4_ren_helper_sem->GetLastValue() };   //
										cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(), Anni::SemInsertInfoSafe(wait_value, curlet_sync_info.stage_mask));
										//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
									}
									{
										//present engine is shitting on the image, wait for it done using it.
										auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->present_finished_helper_sem;
										const auto wait_value = Anni::WaitValue{ present_engine_finshed_presenting->GetLastValue() };   //
										cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(), Anni::SemInsertInfoSafe{ wait_value,curlet_sync_info.stage_mask });
									}
									{
										if (PassType::Present != cur_pass->GetRenderpassType())
										{
											auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(device_manager);
											constexpr auto signal_value = Anni::SignalValue{ 1 };   //
											cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(), Anni::SemInsertInfoSafe{ signal_value,curlet_sync_info.stage_mask });
										}
									}
								}
							}
							else          //�Ѿ����ع��ˣ�
							{
								auto source_sync = Anni::ImgSyncInfo{
									.access_mask = vk::AccessFlagBits2::eNone,
									.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
									.layout_inpass = vk::ImageLayout::eUndefined,
									.img_subrange = std::nullopt
								};
								cur_pass->InsertSyncInfoForInitalUsage(
									source_sync,
									curlet_sync_info,
									cur_let.GetUnderlyingRsrcItr()
								);

								if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
								{
									{
										//maybe some previous frames is using the current image , so wait
										auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->swap_img_rdy_4_rendering_helper_sem;
										const auto wait_value = Anni::WaitValue{ rdy_4_ren_helper_sem->GetLastValue() };   //
										cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(), Anni::SemInsertInfoSafe(wait_value, curlet_sync_info.stage_mask));
										//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
									}
									{
										//present engine is shitting on the image, wait for it done using it.
										auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->present_finished_helper_sem;
										const auto wait_value = Anni::WaitValue{ present_engine_finshed_presenting->GetLastValue() };   //
										cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(), Anni::SemInsertInfoSafe{ wait_value,curlet_sync_info.stage_mask });
									}
									{
										if (PassType::Present != cur_pass->GetRenderpassType())
										{
											auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(device_manager);
											constexpr auto signal_value = Anni::SignalValue{ 1 };   //
											cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(), Anni::SemInsertInfoSafe{ signal_value,curlet_sync_info.stage_mask });
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
								if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState())//˵����û������ɣ������Ļ���������Ҫ�������semaphore��ͬ��ԭ��
								{
									SemInsertInfoSafe sem_insert_info
									{
										Anni::WaitValue{sem_wrapper->GetLastValue()},
										curlet_sync_info.stage_mask
									};

									auto source_sync = p_rsrc->GetSynInfoOnLoad();
									cur_pass->InsertSyncInfoForInitalLoad(
										source_sync,
										curlet_sync_info,
										cur_let.GetUnderlyingRsrcItr(),
										sem_wrapper,
										sem_insert_info
									);
								}
								else //�Ѿ����ع��ˣ�
								{
									auto source_sync = Anni::ImgSyncInfo{
										.access_mask = vk::AccessFlagBits2::eNone,
										.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
										.layout_inpass = vk::ImageLayout::eUndefined,
										.img_subrange = std::nullopt
									};
									cur_pass->InsertSyncInfoForInitalUsage(
										source_sync,
										curlet_sync_info,
										cur_let.GetUnderlyingRsrcItr()
									);
								}
							}
						}
					}
				}

				//�õ�����ʹ�ø���Դ��pass������֮ǰ��ÿһ��layer������ͬ��ԭ����롣���統ǰlayer��5������������Ҫ�ȴ���layer=0,1,2,3,4
				for (auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.level_to_passes_attached_to_partitioned)
				{
					//���ǰ���layer i �����˶�ĳһ��Դͬʱaccess�������
						//layer i ���ܺ��ж��ִ���ڲ�ͬqueue�ϵ�passes�����������ǰ��շֿ�����ģ���������£�����ֻ��Ҫ�����һ���е�����pass�͵�ǰpass֮�����ͬ������
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

							auto founded_tex_usage = std::ranges::find_if(sor_pass->tex_usages,
								[&curlet_underlying_rsrc](const auto& tex_usage)
								{
									return (&(tex_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_tex_usage != sor_pass->tex_usages.end());

							std::variant<TexUsage, AttachUsage>& sor_tex_usage = founded_tex_usage->usage;
							std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

							Anni::ImgSyncInfo sor_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, sor_tex_usage);

							Anni::ImgSyncInfo tar_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, tar_tex_usage);

							auto underlying_tex = target_let.GetUnderlyingRsrcItr();

							//����passִ�е�Queue��ͬ��ֱ����same queue sync info
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

							auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

							auto& target_let = cur_let;
							const auto underlying_tex = target_let.GetUnderlyingRsrcItr();

							auto founded_tex_usage = std::ranges::find_if(sor_pass->tex_usages,
								[&curlet_underlying_rsrc](const auto& tex_usage)
								{
									return (&(tex_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});

							std::variant<TexUsage, AttachUsage>& sor_tex_usage = founded_tex_usage->usage;
							std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

							Anni::ImgSyncInfo sor_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, sor_tex_usage);

							Anni::ImgSyncInfo tar_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, tar_tex_usage);

							//Source pass     ------------------------->       Cur pass
							//Sync primitive
							VULKAN_HPP_ASSERT(founded_tex_usage != sor_pass->tex_usages.end());
							auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });
							const uint64_t sor_signal_val = sync_sema->GetLastValue() + 1;
							const uint64_t tar_wait_val = sync_sema->GetLastValue() + 1;

							Anni::SemInsertInfoSafe sor_insert_info{
								Anni::SignalValue(sync_sema->GetLastValue() + 1),
								sor_tex_sync_ino.stage_mask
							};
							Anni::SemInsertInfoSafe tar_insert_info{
								Anni::WaitValue(tar_wait_val),
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
								underlying_tex,
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
								underlying_tex,
								sor_insert_info,
								tar_insert_info
							);
						}
					}
					//ͬʱ�����ڵ�ǰͬһ��layer������pass������Ҫ��һ��ִ��˳��
					if (level == cur_level)
					{
						//passes_at_same_levels�Ų�һ���� ����queue �ֿ��
						// ����passes_at_same_levels��   pass0 , pass1 , pass4, pass5 , pass7 , pass9
						// ��0  queue3 ��|pass0��pass1��pass4| ;  ��1  queue 2 ��|pass5��pass7��pass9|

						//��ͬһ��level�о������ͬ������ͬһlevel�е�passӦ��ֻ�����ͬʱ��һ����Դ��������������ֻ��Ҫ������ͬqueue��queue ownership transfer������?
						//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);
						auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];


						std::vector<Queue*> unique_queues_ordered;
						std::transform(passes_at_same_level_partitioned.begin(), passes_at_same_level_partitioned.end(), std::back_inserter(unique_queues_ordered),
							[this](const std::vector<GraphicsPassNode*>& inner_pass_group)->Queue*
							{
								return  pass_to_queue_info.at(inner_pass_group.front()).first;
							}
						);

						auto  cur_pass_pos_in_cur_level = std::find(passes_at_same_levels.begin(), passes_at_same_levels.end(), cur_pass);
						auto  cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);


						VULKAN_HPP_ASSERT(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
						VULKAN_HPP_ASSERT(cur_pass_pos_in_cur_level != passes_at_same_levels.end(), "Current pass is not found at the level!");

						auto sor_pass = cur_pass;
						auto tar_pass = cur_pass;

						//�����ǰpass�������level�е�һ��pass
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

							auto founded_tex_usage = std::ranges::find_if(sor_pass->tex_usages,
								[&curlet_underlying_rsrc](const auto& tex_usage)
								{
									return (&(tex_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_tex_usage != sor_pass->tex_usages.end());

							std::variant<TexUsage, AttachUsage>& sor_tex_usage = founded_tex_usage->usage;
							std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

							Anni::ImgSyncInfo sor_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, sor_tex_usage);

							Anni::ImgSyncInfo tar_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, tar_tex_usage);

							//Source pass     ------------------------->       Cur pass
							//Sync primitive
							auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

							Anni::SemInsertInfoSafe sor_insert_info{
								Anni::SignalValue(sync_sema->GetLastValue() + 1),
								sor_tex_sync_ino.stage_mask
							};

							Anni::SemInsertInfoSafe tar_insert_info{
								Anni::WaitValue(sync_sema->GetLastValue() + 1),
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
									target_let.GetUnderlyingRsrcItr()
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
									target_let.GetUnderlyingRsrcItr(),
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
									target_let.GetUnderlyingRsrcItr(),
									sor_insert_info,
									tar_insert_info
								);
							}

						}
					}
				}
			}
		}

		void SyncPrimitivesInsertionOutletTexture(GraphicsPassNode* const cur_pass)
		{
			auto& swap_imgs_finshed_using_by_cur_pass = vec_swap_imgs_finshed_using.back();
			auto cur_level = cur_pass->cur_level;
			//����inlet tex
			for (auto& [_, cur_let] : cur_pass->outs_tex)
			{
				VirtualTexture& curlet_underlying_rsrc = cur_let.GetUnderlyingRsrcItr()->second;
				auto& curlet_usage = cur_let.GetRsrcUsage();
				IRsrcUsage::RsrcOrigin curlet_origin =
					std::visit([](auto& variant_usage) -> IRsrcUsage::RsrcOrigin
						{
							return variant_usage.origin;
						}, curlet_usage);

				Anni::ImgSyncInfo curlet_sync_info =
					std::visit([](auto& variant_usage) -> Anni::ImgSyncInfo
						{
							return variant_usage.GetSynInfo();
						}, curlet_usage);


				//std::shared_ptr<BinarySemWrapper>& curlet_available_sem =
				//	std::visit([](auto& variant_usage) -> std::shared_ptr<BinarySemWrapper>&
				//		{
				//			return variant_usage.GetAvailableSem();
				//		}, curlet_usage);


				//ֱ���ڵ�ǰpass��������Դ��ͬ��ԭ����룺
				if (IRsrcUsage::RsrcOrigin::EstablishedInSitu == curlet_origin)
				{
					auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
					cur_pass->InsertSyncInfoForInitalUsage(
						source_sync,
						curlet_sync_info,
						cur_let.GetUnderlyingRsrcItr()
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

							if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState())//˵����û������ɣ������Ļ���������Ҫ�������semaphore��ͬ��ԭ��
							{
								auto source_sync = curlet_underlying_rsrc.p_rsrc->GetSynInfoOnLoad();
								SemInsertInfoSafe sem_insert_info
								{
									Anni::WaitValue(sem_wrapper->GetLastValue()),
									curlet_sync_info.stage_mask
								};

								cur_pass->InsertSyncInfoForInitalLoad(
									source_sync,
									curlet_sync_info,
									cur_let.GetUnderlyingRsrcItr(),
									sem_wrapper,
									sem_insert_info
								);


								if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
								{
									{
										//maybe some previous frames is using the current image , so wait
										auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->swap_img_rdy_4_rendering_helper_sem;
										const auto wait_value = Anni::WaitValue{ rdy_4_ren_helper_sem->GetLastValue() };   //
										cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(), Anni::SemInsertInfoSafe(wait_value, curlet_sync_info.stage_mask));
										//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
									}
									{
										//present engine is shitting on the image, wait for it done using it.
										auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->present_finished_helper_sem;
										const auto wait_value = Anni::WaitValue{ present_engine_finshed_presenting->GetLastValue() };   //
										cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(), Anni::SemInsertInfoSafe{ wait_value,curlet_sync_info.stage_mask });
									}
									{
										if (PassType::Present != cur_pass->GetRenderpassType())
										{
											auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(device_manager);
											constexpr auto signal_value = Anni::SignalValue{ 1 };   //
											cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(), Anni::SemInsertInfoSafe{ signal_value,curlet_sync_info.stage_mask });
										}
									}
								}

							}
							else          //�Ѿ����ع��ˣ�
							{
								auto source_sync = Anni::ImgSyncInfo{
									.access_mask = vk::AccessFlagBits2::eNone,
									.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
									.layout_inpass = vk::ImageLayout::eUndefined,
									.img_subrange = std::nullopt
								};
								cur_pass->InsertSyncInfoForInitalUsage(
									source_sync,
									curlet_sync_info,
									cur_let.GetUnderlyingRsrcItr()
								);


								if (curlet_underlying_rsrc.p_rsrc->IsSwapTexture())
								{
									{
										//maybe some previous frames is using the current image , so wait
										auto& rdy_4_ren_helper_sem = curlet_underlying_rsrc.p_rsrc->swap_img_rdy_4_rendering_helper_sem;
										const auto wait_value = Anni::WaitValue{ rdy_4_ren_helper_sem->GetLastValue() };   //
										cur_pass->AddTimelineSyncSem(rdy_4_ren_helper_sem->GetRaw(), Anni::SemInsertInfoSafe(wait_value, curlet_sync_info.stage_mask));
										//sync_info.AddSemToWaitOn(rdy_4_ren_helper_sem, wait_value);
									}
									{
										//present engine is shitting on the image, wait for it done using it.
										auto& present_engine_finshed_presenting = curlet_underlying_rsrc.p_rsrc->present_finished_helper_sem;
										const auto wait_value = Anni::WaitValue{ present_engine_finshed_presenting->GetLastValue() };   //
										cur_pass->AddBinarySemSync(present_engine_finshed_presenting->GetRaw(), Anni::SemInsertInfoSafe{ wait_value,curlet_sync_info.stage_mask });
									}
									{
										if (PassType::Present != cur_pass->GetRenderpassType())
										{
											auto& img_finished_sem = swap_imgs_finshed_using_by_cur_pass.emplace_back(device_manager);
											constexpr auto signal_value = Anni::SignalValue{ 1 };   //
											cur_pass->AddBinarySemSync(img_finished_sem->GetRaw(), Anni::SemInsertInfoSafe{ signal_value,curlet_sync_info.stage_mask });
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
								if (sem_wrapper->GetLastValue() > sem_wrapper->ReturnState())//˵����û������ɣ������Ļ���������Ҫ�������semaphore��ͬ��ԭ��
								{
									SemInsertInfoSafe sem_insert_info
									{
										Anni::WaitValue(sem_wrapper->GetLastValue()),
										curlet_sync_info.stage_mask
									};

									auto source_sync = p_rsrc->GetSynInfoOnLoad();
									cur_pass->InsertSyncInfoForInitalLoad(
										source_sync,
										curlet_sync_info,
										cur_let.GetUnderlyingRsrcItr(),
										sem_wrapper,
										sem_insert_info
									);
								}
								else //�Ѿ����ع��ˣ�
								{
									auto source_sync = Anni::ImgSyncInfo{
										.access_mask = vk::AccessFlagBits2::eNone,
										.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
										.layout_inpass = vk::ImageLayout::eUndefined,
										.img_subrange = std::nullopt
									};
									cur_pass->InsertSyncInfoForInitalUsage(
										source_sync,
										curlet_sync_info,
										cur_let.GetUnderlyingRsrcItr()
									);
								}
							}
						}
					}
				}


				//�õ�����ʹ�ø���Դ��pass������֮ǰ��ÿһ��layer������ͬ��ԭ����롣���統ǰlayer��5������������Ҫ�ȴ���layer=0,1,2,3,4
				for (auto& [level, passes_at_same_level_partitioned] : curlet_underlying_rsrc.level_to_passes_attached_to_partitioned)
				{
					//���ǰ���layer i �����˶�ĳһ��Դͬʱaccess�������
						//layer i ���ܺ��ж��ִ���ڲ�ͬqueue�ϵ�passes�����������ǰ��շֿ�����ģ���������£�����ֻ��Ҫ�����һ���е�����pass�͵�ǰpass֮�����ͬ������
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

							auto founded_tex_usage = std::ranges::find_if(sor_pass->tex_usages,
								[&curlet_underlying_rsrc](const auto& tex_usage)
								{
									return (&(tex_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_tex_usage != sor_pass->tex_usages.end());

							std::variant<TexUsage, AttachUsage>& sor_tex_usage = founded_tex_usage->usage;
							std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

							Anni::ImgSyncInfo sor_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, sor_tex_usage);

							Anni::ImgSyncInfo tar_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, tar_tex_usage);

							auto underlying_tex = target_let.GetUnderlyingRsrcItr();

							//����passִ�е�Queue��ͬ��ֱ����same queue sync info
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
							const auto underlying_tex = target_let.GetUnderlyingRsrcItr();

							auto founded_tex_usage = std::ranges::find_if(sor_pass->tex_usages,
								[&curlet_underlying_rsrc](const auto& tex_usage)
								{
									return (&(tex_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});

							std::variant<TexUsage, AttachUsage>& sor_tex_usage = founded_tex_usage->usage;
							std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

							Anni::ImgSyncInfo sor_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, sor_tex_usage);

							Anni::ImgSyncInfo tar_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, tar_tex_usage);

							//Source pass     ------------------------->       Cur pass
							//Sync primitive
							assert(founded_tex_usage != sor_pass->tex_usages.end());
							auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

							Anni::SemInsertInfoSafe sor_insert_info{
								Anni::SignalValue(sync_sema->GetLastValue() + 1),
								sor_tex_sync_ino.stage_mask
							};
							Anni::SemInsertInfoSafe tar_insert_info{
								Anni::WaitValue(sync_sema->GetLastValue() + 1),
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
								underlying_tex,
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
								underlying_tex,
								sor_insert_info,
								tar_insert_info
							);
						}
					}
					//ͬʱ�����ڵ�ǰͬһ��layer������pass������Ҫ��һ��ִ��˳��
					if (level == cur_level)
					{
						//passes_at_same_levels�Ų�һ���� ����queue �ֿ��
						// ����passes_at_same_levels��   pass0 , pass1 , pass4, pass5 , pass7 , pass9
						// ��0  queue3 ��|pass0��pass1��pass4| ;  ��1  queue 2 ��|pass5��pass7��pass9|

						//��ͬһ��level�о������ͬ������ͬһlevel�е�passӦ��ֻ�����ͬʱ��һ����Դ��������������ֻ��Ҫ������ͬqueue��queue ownership transfer������?
						//auto& sync_timeline_sema_same_level = same_level_sync_sema.at(level);
						auto& passes_at_same_levels = curlet_underlying_rsrc.level_to_passes_attached_to[level];


						std::vector<Queue*> unique_queues_ordered;
						std::transform(passes_at_same_level_partitioned.begin(), passes_at_same_level_partitioned.end(), std::back_inserter(unique_queues_ordered),
							[this](const std::vector<GraphicsPassNode*>& inner_pass_group)->Queue*
							{
								return  pass_to_queue_info.at(inner_pass_group.front()).first;
							}
						);

						auto  cur_pass_pos_in_cur_level = std::find(passes_at_same_levels.begin(), passes_at_same_levels.end(), cur_pass);
						auto  cur_queue_itr = std::ranges::find(unique_queues_ordered, pass_to_queue_info.at(cur_pass).first);


						VULKAN_HPP_ASSERT(cur_queue_itr != unique_queues_ordered.end(), "Queue for current pass not found!");
						VULKAN_HPP_ASSERT(cur_pass_pos_in_cur_level != passes_at_same_levels.end(), "Current pass is not found at the level!");

						auto sor_pass = cur_pass;
						auto tar_pass = cur_pass;

						//�����ǰpass�������level�е�һ��pass
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

							auto founded_tex_usage = std::ranges::find_if(sor_pass->tex_usages,
								[&curlet_underlying_rsrc](const auto& tex_usage)
								{
									return (&(tex_usage.v_rsrc->second) == &(curlet_underlying_rsrc));
								});
							assert(founded_tex_usage != sor_pass->tex_usages.end());

							std::variant<TexUsage, AttachUsage>& sor_tex_usage = founded_tex_usage->usage;
							std::variant<TexUsage, AttachUsage>& tar_tex_usage = target_let.GetRsrcUsage();

							Anni::ImgSyncInfo sor_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, sor_tex_usage);

							Anni::ImgSyncInfo tar_tex_sync_ino =
								std::visit([&](auto& variant_usage) -> Anni::ImgSyncInfo
									{
										return variant_usage.GetSynInfo();
									}, tar_tex_usage);

							//Source pass     ------------------------->       Cur pass
							//Sync primitive
							auto& sync_sema = diff_queue_sync_sema.at(PassNodePair{ sor_pass, tar_pass });

							Anni::SemInsertInfoSafe sor_insert_info{
								Anni::SignalValue(sync_sema->GetLastValue() + 1),
								sor_tex_sync_ino.stage_mask
							};
							Anni::SemInsertInfoSafe tar_insert_info{
								Anni::WaitValue(sync_sema->GetLastValue() + 1),
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
									target_let.GetUnderlyingRsrcItr()
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
									target_let.GetUnderlyingRsrcItr(),
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
									target_let.GetUnderlyingRsrcItr(),
									sor_insert_info,
									tar_insert_info
								);
							}

						}
					}
				}
			}
		}

		void SyncPrimitivesInsertion()
		{
			vec_swap_imgs_finshed_using.push_back(std::vector<std::shared_ptr<BinarySemWrapper>>{});
			///************************************************************
			//SYNC PRIMITIVES INSERTIONͬ��ԭ�����:
			///************************************************************
			//����level�����loop��
			for (auto cur_level = 0; cur_level <= max_level; ++cur_level)
			{
				for (auto cur_pass : topologically_sorted_nodes)
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



	private:
		//ȫ֪ȫ�ܵ�������Դ������
		std::unordered_map<std::string, VirtualBuffer>  rg_buffers_map;
		std::unordered_map<std::string, VirtualTexture> rg_textures_map;
		//��ǰ֡���е�pass
		std::vector<std::unique_ptr<GraphicsPassNode>> pass_nodes;
		//���������Ժ������passnode	
		std::vector<GraphicsPassNode*>                 topologically_sorted_nodes;

		//��ǰ֡������pass���㼶
		using Level = size_t;
		Level         max_level{ 0 };
		std::unordered_map<Level, bool>                if_level_accesses_same_rsrc_multimes;
		std::map<Level, std::vector<GraphicsPassNode*>> level_to_all_passes_attached;

		//pass��queue�ϵ�ִ����Ϣ
		using MonotonicalAssignedIndex = size_t;
		std::unordered_map<Queue*, std::vector<GraphicsPassNode*>>                         queue_to_passes_on_it;
		std::unordered_map<GraphicsPassNode*, std::pair<Queue*, MonotonicalAssignedIndex>> pass_to_queue_info;

		//ͬ����Ϣ
		using DependencyLevel = size_t;
		std::unordered_map<PassNodePair, std::shared_ptr<TimelineSemWrapper>> diff_queue_sync_sema;

		//swap imageͬ����Ϣ
		std::vector<std::vector<std::shared_ptr<BinarySemWrapper>>> vec_swap_imgs_finshed_using;




	private:
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		QueueManager& queue_manager;
		DescriptorLayoutManager& descriptor_set_layout_manager;

		DescriptorAllocatorGrowable& descriptor_allocator;
		VkTimelineSemPoolUnsafe semaphore_pool;

		BufferFactory& buf_fac;
		VkTextureFactory& tex_fac;
		VkShaderFactory& shader_fac;

		GFXPipelineCIBuilder& gfx_pipelineCI_builder;

	};

}