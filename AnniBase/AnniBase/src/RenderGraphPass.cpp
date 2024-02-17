#include "RenderGraphPass.h"
namespace Anni
{
//
//RenderGraphV0::PassNode::PassNode(const std::string name_, GraphicsComponent &gfx_) :
//    name(name_), root_pass_tag(nullptr), is_rootpass(false), gfx(gfx_)
//{
//}
//
//RenderGraphV0::PassNode::~PassNode()
//{
//}
//
//void RenderGraphV0::PassNode::SetAsRootPass()
//{
//	is_rootpass = true;
//}
//
//void RenderGraphV0::PassNode::SetAsNonRootPass()
//{
//	is_rootpass = false;
//}
//
//bool RenderGraphV0::PassNode::IsRootPass()
//{
//	return is_rootpass;
//}
//
//void RenderGraphV0::PassNode::TagCurRootPass(PassNode *cur_pass)
//{
//	root_pass_tag = cur_pass;
//}
//
//void RenderGraphV0::PassNode::AddSubGraphPass(PassNode *ps)
//{
//	if (!is_rootpass)
//	{
//		throw std::runtime_error("This is not a root pass");
//	}
//
//	subgraph_pass.insert(ps);
//}
//
//VkRenderpassBaseRG::Type RenderGraphV0::PassNode::GetAccumulatedType()
//{
//	return subgraph_accumulated_type;
//}
//
//void RenderGraphV0::PassNode::AccumulateQueueRequirement()
//{
//	if (!is_rootpass)
//	{
//		subgraph_accumulated_type = VkRenderpassBaseRG::Type::None;
//	}
//	else
//	{
//		subgraph_accumulated_type = GetRenderpassType();
//		for (auto pass : subgraph_pass)
//		{
//			subgraph_accumulated_type = static_cast<VkRenderpassBaseRG::Type>((subgraph_accumulated_type | pass->GetRenderpassType()));
//		}
//	}
//}
//
//VkRenderpassBaseRG::Type RenderGraphV0::PassNode::GetRenderpassType()
//{
//	return VkRenderpassBaseRG::Type::None;
//}
//
//void RenderGraphV0::PassNode::InitRsrc()
//{
//	//REQUIRED RESOURCES INITIALIZATION
//	for (auto &buf : buf_init_list)
//	{
//		//VirtualResource has a descriptor to help initalize resources
//		buf->second.Actualize(renderpass_manager);
//	}
//
//	for (auto &tex : uni_tex_init_list)
//	{
//		//VirtualResource has a descriptor to help initalize resources
//		tex->second.Actualize(renderpass_manager);
//	}
//}
//
//void RenderGraphV0::PassNode::SyncInsertAhead(VkCommandBuffer cmd_buf)
//{
//	//SAME QUEUE SYN同步。TODO:合批
//	// 暂时不用event做同步。
//
//	for (auto &head_syn : buf_syn_infos_head_same_q)
//	{
//		//std::vector<VkDependencyInfo> depens;
//
//		VirtualResource<Buffer> &buff   = head_syn.underlying_rsrc->second;
//		VkBufferMemoryBarrier2   buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//
//		VkDependencyInfo depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 1,
//		    .pBufferMemoryBarriers    = &buf_barrier,
//		    .imageMemoryBarrierCount  = 0,
//		    .pImageMemoryBarriers     = VK_NULL_HANDLE};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//		//vkCmdWaitEvents2(cmd_buf, 1, &head_syn.sync_event, depens.data());
//	}
//
//	for (auto &head_syn : uni_tex_syn_infos_head_same_q)
//	{
//		//std::vector<VkDependencyInfo> depens;
//		VirtualResource<VkTexture> &tex         = head_syn.underlying_rsrc->second;
//		auto                        tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//
//		auto depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 0,
//		    .pBufferMemoryBarriers    = VK_NULL_HANDLE,
//		    .imageMemoryBarrierCount  = 1,
//		    .pImageMemoryBarriers     = &tex_barrier};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//		//vkCmdWaitEvents2(cmd_buf, 1, &head_syn.sync_event, depens.data());
//	}
//
//	//DIFF QUEUE 同步primitive 可能需要queue ownership transfer
//
//	//Semaphore wait/signal 需要在指令录制完以后，用VkQueueSubmit提交
//
//	for (auto &head_syn : buf_syn_infos_head_diff_q)
//	{
//		VirtualResource<Buffer> &buff               = head_syn.underlying_rsrc->second;
//		auto                           buf_barrier        = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//		size_t                         source_q_fam_index = head_syn.source_queue->GetQueueCap().queue_family_index;
//		size_t                         target_q_fam_index = head_syn.target_queue->GetQueueCap().queue_family_index;
//
//		if (source_q_fam_index != target_q_fam_index)
//		{
//			buf_barrier.srcQueueFamilyIndex = source_q_fam_index;
//			buf_barrier.dstQueueFamilyIndex = target_q_fam_index;
//		}
//
//		//constexpr uint64_t wait_value       = 1;
//		//auto               sema_submit_info = VkSemaphoreSubmitInfo{
//		//          .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//		//          .pNext       = VK_NULL_HANDLE,
//		//          .semaphore   = head_syn.sync_sema,
//		//          .value       = wait_value,
//		//          .stageMask   = head_syn.target_sync_info.stage_mask,
//		//          .deviceIndex = 0};
//		//auto submit_info = VkSubmitInfo2{
//		//    .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
//		//    .pNext                    = VK_NULL_HANDLE,
//		//    .flags                    = 0,
//		//    .waitSemaphoreInfoCount   = 1,
//		//    .pWaitSemaphoreInfos      = &sema_submit_info,
//		//    .commandBufferInfoCount   = 0,
//		//    .pCommandBufferInfos      = VK_NULL_HANDLE,
//		//    .signalSemaphoreInfoCount = 0,
//		//    .pSignalSemaphoreInfos    = VK_NULL_HANDLE};
//
//		//vkQueueSubmit2(head_syn.target_queue->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
//
//		auto depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 1,
//		    .pBufferMemoryBarriers    = &buf_barrier,
//		    .imageMemoryBarrierCount  = 0,
//		    .pImageMemoryBarriers     = VK_NULL_HANDLE};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//	}
//
//	for (auto &head_syn : uni_tex_syn_infos_head_diff_q)
//	{
//		//std::vector<VkDependencyInfo> depens;
//
//		VirtualResource<VkTexture> &tex                = head_syn.underlying_rsrc->second;
//		auto                        tex_barrier        = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//		size_t                      source_q_fam_index = head_syn.source_queue->GetQueueCap().queue_family_index;
//		size_t                      target_q_fam_index = head_syn.target_queue->GetQueueCap().queue_family_index;
//
//		if (source_q_fam_index != target_q_fam_index)
//		{
//			tex_barrier.srcQueueFamilyIndex = source_q_fam_index;
//			tex_barrier.dstQueueFamilyIndex = target_q_fam_index;
//		}
//
//		//constexpr uint64_t wait_value       = 1;
//		//auto               sema_submit_info = VkSemaphoreSubmitInfo{
//		//          .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//		//          .pNext       = VK_NULL_HANDLE,
//		//          .semaphore   = head_syn.sync_sema,
//		//          .value       = wait_value,
//		//          .stageMask   = head_syn.target_sync_info.stage_mask,
//		//          .deviceIndex = 0};
//		//auto submit_info = VkSubmitInfo2{
//		//    .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
//		//    .pNext                    = VK_NULL_HANDLE,
//		//    .flags                    = 0,
//		//    .waitSemaphoreInfoCount   = 1,
//		//    .pWaitSemaphoreInfos      = &sema_submit_info,
//		//    .commandBufferInfoCount   = 0,
//		//    .pCommandBufferInfos      = VK_NULL_HANDLE,
//		//    .signalSemaphoreInfoCount = 0,
//		//    .pSignalSemaphoreInfos    = VK_NULL_HANDLE};
//
//		//vkQueueSubmit2(head_syn.target_queue->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
//
//		auto depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 0,
//		    .pBufferMemoryBarriers    = VK_NULL_HANDLE,
//		    .imageMemoryBarrierCount  = 1,
//		    .pImageMemoryBarriers     = &tex_barrier};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//	}
//}
//
//void RenderGraphV0::PassNode::SyncInsertAfter(VkCommandBuffer cmd_buf)
//{
//	for (auto &head_syn : buf_syn_infos_tail_same_q)
//	{
//		//std::vector<VkDependencyInfo> depens;
//		VirtualResource<Buffer> &buff        = head_syn.underlying_rsrc->second;
//		VkBufferMemoryBarrier2         buf_barrier = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//
//		VkDependencyInfo depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 1,
//		    .pBufferMemoryBarriers    = &buf_barrier,
//		    .imageMemoryBarrierCount  = 0,
//		    .pImageMemoryBarriers     = VK_NULL_HANDLE};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//		//vkCmdWaitEvents2(cmd_buf, 1, &head_syn.sync_event, depens.data());
//	}
//
//	for (auto &head_syn : uni_tex_syn_infos_tail_same_q)
//	{
//		//std::vector<VkDependencyInfo> depens;
//		VirtualResource<VkTexture> &tex         = head_syn.underlying_rsrc->second;
//		auto                        tex_barrier = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//
//		auto depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 0,
//		    .pBufferMemoryBarriers    = VK_NULL_HANDLE,
//		    .imageMemoryBarrierCount  = 1,
//		    .pImageMemoryBarriers     = &tex_barrier};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//		//vkCmdWaitEvents2(cmd_buf, 1, &head_syn.sync_event, depens.data());
//	}
//
//	//DIFF QUEUE 同步primitive 可能需要queue ownership transfer
//
//	//Semaphore wait/signal 需要在指令录制完以后，用VkQueueSubmit提交
//
//	for (auto &head_syn : buf_syn_infos_tail_diff_q)
//	{
//		VirtualResource<Buffer> &buff               = head_syn.underlying_rsrc->second;
//		auto                           buf_barrier        = buff.GetBufBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//		size_t                         source_q_fam_index = head_syn.source_queue->GetQueueCap().queue_family_index;
//		size_t                         target_q_fam_index = head_syn.target_queue->GetQueueCap().queue_family_index;
//
//		if (source_q_fam_index != target_q_fam_index)
//		{
//			buf_barrier.srcQueueFamilyIndex = source_q_fam_index;
//			buf_barrier.dstQueueFamilyIndex = target_q_fam_index;
//		}
//
//		//constexpr uint64_t wait_value       = 1;
//		//auto               sema_submit_info = VkSemaphoreSubmitInfo{
//		//          .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//		//          .pNext       = VK_NULL_HANDLE,
//		//          .semaphore   = head_syn.sync_sema,
//		//          .value       = wait_value,
//		//          .stageMask   = head_syn.target_sync_info.stage_mask,
//		//          .deviceIndex = 0};
//		//auto submit_info = VkSubmitInfo2{
//		//    .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
//		//    .pNext                    = VK_NULL_HANDLE,
//		//    .flags                    = 0,
//		//    .waitSemaphoreInfoCount   = 1,
//		//    .pWaitSemaphoreInfos      = &sema_submit_info,
//		//    .commandBufferInfoCount   = 0,
//		//    .pCommandBufferInfos      = VK_NULL_HANDLE,
//		//    .signalSemaphoreInfoCount = 0,
//		//    .pSignalSemaphoreInfos    = VK_NULL_HANDLE};
//
//		//vkQueueSubmit2(head_syn.target_queue->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
//
//		auto depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 1,
//		    .pBufferMemoryBarriers    = &buf_barrier,
//		    .imageMemoryBarrierCount  = 0,
//		    .pImageMemoryBarriers     = VK_NULL_HANDLE};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//	}
//
//	for (auto &head_syn : uni_tex_syn_infos_tail_diff_q)
//	{
//		//std::vector<VkDependencyInfo> depens;
//
//		VirtualResource<VkTexture> &tex                = head_syn.underlying_rsrc->second;
//		auto                        tex_barrier        = tex.GetTexBarrier(head_syn.source_sync_info, head_syn.target_sync_info);
//		size_t                      source_q_fam_index = head_syn.source_queue->GetQueueCap().queue_family_index;
//		size_t                      target_q_fam_index = head_syn.target_queue->GetQueueCap().queue_family_index;
//
//		if (source_q_fam_index != target_q_fam_index)
//		{
//			tex_barrier.srcQueueFamilyIndex = source_q_fam_index;
//			tex_barrier.dstQueueFamilyIndex = target_q_fam_index;
//		}
//
//		//constexpr uint64_t wait_value       = 1;
//		//auto               sema_submit_info = VkSemaphoreSubmitInfo{
//		//          .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//		//          .pNext       = VK_NULL_HANDLE,
//		//          .semaphore   = head_syn.sync_sema,
//		//          .value       = wait_value,
//		//          .stageMask   = head_syn.target_sync_info.stage_mask,
//		//          .deviceIndex = 0};
//		//auto submit_info = VkSubmitInfo2{
//		//    .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
//		//    .pNext                    = VK_NULL_HANDLE,
//		//    .flags                    = 0,
//		//    .waitSemaphoreInfoCount   = 1,
//		//    .pWaitSemaphoreInfos      = &sema_submit_info,
//		//    .commandBufferInfoCount   = 0,
//		//    .pCommandBufferInfos      = VK_NULL_HANDLE,
//		//    .signalSemaphoreInfoCount = 0,
//		//    .pSignalSemaphoreInfos    = VK_NULL_HANDLE};
//
//		//vkQueueSubmit2(head_syn.target_queue->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
//
//		auto depen_info = VkDependencyInfo{
//		    .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//		    .pNext                    = VK_NULL_HANDLE,
//		    .dependencyFlags          = VK_DEPENDENCY_BY_REGION_BIT,
//		    .memoryBarrierCount       = 0,
//		    .pMemoryBarriers          = VK_NULL_HANDLE,
//		    .bufferMemoryBarrierCount = 0,
//		    .pBufferMemoryBarriers    = VK_NULL_HANDLE,
//		    .imageMemoryBarrierCount  = 1,
//		    .pImageMemoryBarriers     = &tex_barrier};
//
//		vkCmdPipelineBarrier2(cmd_buf, &depen_info);
//	}
//}
//
//void RenderGraphV0::PassNode::InsertInitalSyncInfo(Vk::BufSyncInfo source_syn_info, Vk::BufSyncInfo target_syn_info, std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc)
//{
//	buf_syn_infos_head_same_q.emplace_back(VK_NULL_HANDLE, source_syn_info, target_syn_info, underlying_rsrc);
//}
//
//void RenderGraphV0::PassNode::InsertInitalSyncInfo(Vk::TexSyncInfo source_syn_info, Vk::TexSyncInfo target_syn_info, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc)
//{
//	uni_tex_syn_infos_head_same_q.emplace_back(VK_NULL_HANDLE, source_syn_info, target_syn_info, underlying_rsrc);
//}
//
//void RenderGraphV0::PassNode::InsertSameQueueSyncInfo(
//    PassNode *                                                               source_pass,
//    PassNode *                                                               target_pass,
//    VkEvent                                                                  event_sync,
//    DeviceManager::QueueCapability                                           queue_info,
//    Vk::BufSyncInfo                                                          source_syn_info,
//    Vk::BufSyncInfo                                                          target_syn_info,
//    std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc)
//{
//	if (this == target_pass)
//	{
//		buf_syn_infos_head_same_q.emplace_back(event_sync, source_syn_info, target_syn_info, underlying_rsrc);
//	}
//	else
//	{
//		throw std::runtime_error("source or target pass specified in a wrong way.");
//	}
//}
//
//void RenderGraphV0::PassNode::InsertSameQueueSyncInfo(
//    PassNode *                                                            source_pass,
//    PassNode *                                                            target_pass,
//    VkEvent                                                               event_sync,
//    DeviceManager::QueueCapability                                      queue_info,
//    Vk::TexSyncInfo                                                       source_syn_info,
//    Vk::TexSyncInfo                                                       target_syn_info,
//    std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc)
//{
//	if (this == target_pass)
//	{
//		uni_tex_syn_infos_head_same_q.emplace_back(event_sync, source_syn_info, target_syn_info, underlying_rsrc);
//	}
//	else
//	{
//		throw std::runtime_error("source or target pass specified in a wrong way.");
//	}
//}
//
////Watch out for queue ownership transfer!
//void RenderGraphV0::PassNode::InsertDiffQueueSyncInfoTimeline(
//    PassNode *                                                               source_pass,
//    PassNode *                                                               target_pass,
//    VkSemaphore                                                              sema_sync,
//    uint64_t                                                                 wait_value,
//    uint64_t                                                                 signal_value,
//    DeviceManager::VkExecutionQueue *                                      source_queue_,
//    DeviceManager::VkExecutionQueue *                                      target_queue_,
//    Vk::BufSyncInfo                                                          source_syn_info,
//    Vk::BufSyncInfo                                                          target_syn_info,
//    std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc
//
//)
//{
//	if (this == source_pass)
//	{
//		buf_syn_infos_tail_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, signal_value, std::nullopt);
//	}
//	else if (this == target_pass)
//	{
//		buf_syn_infos_head_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, std::nullopt, wait_value);
//	}
//	else
//	{
//		throw std::runtime_error("source or target pass specified in a wrong way.");
//	}
//}
//
//void RenderGraphV0::PassNode::InsertDiffQueueSyncInfoTimeline(
//    PassNode *                                                            source_pass,
//    PassNode *                                                            target_pass,
//    VkSemaphore                                                           sema_sync,
//    uint64_t                                                              wait_value,
//    uint64_t                                                              signal_value,
//    DeviceManager::VkExecutionQueue *                                   source_queue_,
//    DeviceManager::VkExecutionQueue *                                   target_queue_,
//    Vk::BufSyncInfo                                                       source_syn_info,
//    Vk::BufSyncInfo                                                       target_syn_info,
//    std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc
//
//)
//{
//	if (this == source_pass)
//	{
//		uni_tex_syn_infos_head_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, signal_value, std::nullopt);
//	}
//	else if (this == target_pass)
//	{
//		uni_tex_syn_infos_head_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, std::nullopt, wait_value);
//	}
//	else
//	{
//		throw std::runtime_error("source or target pass specified in a wrong way.");
//	}
//}
//
//void RenderGraphV0::PassNode::InsertDiffQueueSyncInfoBinarySem(
//    PassNode *                                                               source_pass,
//    PassNode *                                                               target_pass,
//    VkSemaphore                                                              sema_sync,
//    DeviceManager::VkExecutionQueue *                                      source_queue_,
//    DeviceManager::VkExecutionQueue *                                      target_queue_,
//    Vk::BufSyncInfo                                                          source_syn_info,
//    Vk::BufSyncInfo                                                          target_syn_info,
//    std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc)
//{
//	if (this == source_pass)
//	{
//		buf_syn_infos_tail_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, 1, std::nullopt);
//	}
//	else if (this == target_pass)
//	{
//		buf_syn_infos_head_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, std::nullopt, 1);
//	}
//	else
//	{
//		throw std::runtime_error("source or target pass specified in a wrong way.");
//	}
//}
//
//void RenderGraphV0::PassNode::InsertDiffQueueSyncInfoBinarySem(
//    PassNode *                                                            source_pass,
//    PassNode *                                                            target_pass,
//    VkSemaphore                                                           sema_sync,
//    DeviceManager::VkExecutionQueue *                                   source_queue_,
//    DeviceManager::VkExecutionQueue *                                   target_queue_,
//    Vk::TexSyncInfo                                                       source_syn_info,
//    Vk::TexSyncInfo                                                       target_syn_info,
//    std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc)
//{
//	if (this == source_pass)
//	{
//		uni_tex_syn_infos_tail_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, 1, std::nullopt);
//	}
//	else if (this == target_pass)
//	{
//		uni_tex_syn_infos_head_diff_q.emplace_back(sema_sync, source_syn_info, target_syn_info, source_queue_, target_queue_, underlying_rsrc, std::nullopt, 1);
//	}
//	else
//	{
//		throw std::runtime_error("source or target pass specified in a wrong way.");
//	}
//}

}
