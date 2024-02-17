#include "RenderGraphV0.h"

//
//namespace Anni
//{
//
//	RenderGraphV0::DependencyGraph::DependencyGraph(const DeviceManager& device_man_, VkRenderpassManager& renderpass_manager_) :
//		device_man(device_man_), renderpass_manager(renderpass_manager_)
//	{
//	}
//
//	VkAttachmentInfo RenderGraphV0::DependencyGraph::GetAttachmentRsrcIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
//		if (!pass->ins_uni_tex.contains(inlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->ins_uni_tex.at(inlet_name);
//		const auto rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		const VkAttachmentInfo::WithinPass attach_info_inpass{ *rsrc_usage };
//
//		return VkAttachmentInfo{ attach_info_inpass, virtual_rsrc.GetUnderlyingRsrcItr()->second.ptr_rsrc };
//	}
//
//	VkAttachmentInfo RenderGraphV0::DependencyGraph::GetAttachmentRsrcOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
//		if (!pass->outs_uni_tex.contains(outlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		const VkAttachmentInfo::WithinPass attach_info_inpass{ *rsrc_usage };
//		return VkAttachmentInfo{ attach_info_inpass, virtual_rsrc.GetUnderlyingRsrcItr()->second.ptr_rsrc };
//	}
//
//	VkTexUsageInfo RenderGraphV0::DependencyGraph::GetTextureRsrcIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
//		if (!pass->ins_uni_tex.contains(inlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->ins_uni_tex.at(inlet_name);
//
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		return VkTexUsageInfo{ rsrc_usage->tex_type, rsrc_usage->shader_stages, std::get<Vk::DescSetInfo>(rsrc_usage->binding_info), virtual_rsrc.GetUnderlyingRsrcItr()->second.ptr_rsrc };
//	}
//
//	VkTexUsageInfo RenderGraphV0::DependencyGraph::GetTextureRsrcOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
//		if (!pass->outs_uni_tex.contains(outlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		return VkTexUsageInfo{ rsrc_usage->tex_type, rsrc_usage->shader_stages, std::get<Vk::DescSetInfo>(rsrc_usage->binding_info), virtual_rsrc.GetUnderlyingRsrcItr()->second.ptr_rsrc };
//	}
//
//	VkUniBufUsageInfo RenderGraphV0::DependencyGraph::GetBufferRsrcIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
//		if (!pass->ins_buf.contains(inlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->ins_buf.at(inlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		return VkUniBufUsageInfo{ rsrc_usage->buf_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetUnderlyingRsrcItr()->second.ptr_rsrc };
//	}
//
//	VkUniBufUsageInfo RenderGraphV0::DependencyGraph::GetBufferRsrcOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
//		if (!pass->outs_buf.contains(outlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->outs_buf.at(outlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		return VkUniBufUsageInfo{ rsrc_usage->buf_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetUnderlyingRsrcItr()->second.ptr_rsrc };
//	}
//
//	void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingAttachmentOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
//		if (!pass->outs_uni_tex.contains(outlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
//		const auto rsrc_usage = virtual_rsrc.GetRsrcUsage();
//		auto& tex_vrsrc = virtual_rsrc.GetUnderlyingRsrcItr()->second;
//
//		RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
//	}
//
//	void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingAttachmentIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
//		if (!pass->ins_uni_tex.contains(inlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->ins_uni_tex.at(inlet_name);
//		const auto rsrc_usage = virtual_rsrc.GetRsrcUsage();
//
//		auto& tex_vrsrc = virtual_rsrc.GetUnderlyingRsrcItr()->second;
//		RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
//	}
//
//	void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingTextureIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
//		if (!pass->ins_uni_tex.contains(inlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->ins_uni_tex.at(inlet_name);
//
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//		auto& tex_vrsrc = virtual_rsrc.GetUnderlyingRsrcItr()->second;
//
//		RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
//	}
//
//	void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingTextureOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
//		if (!pass->outs_uni_tex.contains(outlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//		auto& tex_vrsrc = virtual_rsrc.GetUnderlyingRsrcItr()->second;
//
//		RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
//	}
//
//	void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingBufferIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
//		if (!pass->ins_buf.contains(inlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->ins_buf.at(inlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//		auto& buf_vrsrc = virtual_rsrc.GetUnderlyingRsrcItr()->second;
//
//		RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, buf_vrsrc, *rsrc_usage);
//	}
//
//	void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingBufferOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index)
//	{
//		const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
//		if (!pass->outs_buf.contains(outlet_name))
//		{
//			throw std::runtime_error("pass doesn't provide required resource");
//		}
//		auto& virtual_rsrc = pass->outs_buf.at(outlet_name);
//		const auto& rsrc_usage = virtual_rsrc.GetRsrcUsage();
//		auto& buf_vrsrc = virtual_rsrc.GetUnderlyingRsrcItr()->second;
//
//		RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, buf_vrsrc, *rsrc_usage);
//	}
//
//	RenderGraphV0::GraphicsPassNode& RenderGraphV0::DependencyGraph::AddGfxPassNode(std::string name, std::shared_ptr<VkRenderpassBaseRG> renderpass_ptr)
//	{
//		auto gfx_node = std::make_unique<GraphicsPassNode>(name, renderpass_ptr, this->buffers_map, this->uni_textures_map);
//		gfx_pass_nodes.emplace_back(std::move(gfx_node));
//		const auto& result = gfx_pass_nodes.back();
//		renderpass_ptr->SetRenderGraphPassHandle(gfx_pass_nodes.size() - 1u);
//		renderpass_ptr->SetRenderGraphPassPointer(result.get());
//
//		return *result;
//	}
//
//	bool RenderGraphV0::DependencyGraph::Compile(const DeviceManager& device_man)
//	{
//		//TODO: Trim all invalid passes and unused resources
//		topologically_sorted_nodes.clear();        //清除最后的执行顺序。
//		GeneratePassNodeDepen();                   //根据In,Out函数声明确定可以推断出来的顺序。
//		TopologicalDfsSort();                      //排序
//		SortPassesAccessingRsrc();                 //好像不必要
//		DependencyLevelsGeneration();
//		GPUWorkDistribution();
//		SyncPrimitivesAquisition();
//		SortPassesAtSameLevel();
//		SyncPrimitivesInsertion();
//		//Render graph compilation finished.
//		return true;
//	}
//
//	void RenderGraphV0::DependencyGraph::ExecutePass(VkCommandBuffer cmd_buf, PassNode* pass)
//	{
//		////REQUIRED RESOURCES INITIALIZATION
//		//for (auto &buf : pass->buf_init_list)
//		//{
//		//	//VirtualResource has a descriptor to help initalize resources
//		//	buf->second.Actualize(renderpass_manager);
//		//}
//
//		//for (auto &tex : pass->uni_tex_init_list)
//		//{
//		//	//VirtualResource has a descriptor to help initalize resources
//		//	tex->second.Actualize(renderpass_manager);
//		//}
//
//		////BARRIERS INSERSION before PASS EXECUTION
//		//for (auto &[source_syn, target_syn, resource_itor] : pass->buf_syn_infos)
//		//{
//		//	//不同资源insert barrier的方法不同，这里可以根据类型做特化；
//		//	resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
//		//}
//
//		//for (auto &[source_syn, target_syn, resource_itor] : pass->uni_tex_syn_infos)
//		//{
//		//	//不同资源insert barrier的方法不同，这里可以根据类型做特化；
//		//	resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
//		//}
//
//		////RENDERPASS INITIALIZATION AND EXECUTION
//		//pass->PreExecute();
//		//pass->Execute(cmd_buf);
//
//		////TRANSIENT RESOURCES DESTRUCTION
//		//for (const auto &rsrc_buf : pass->buf_recycle_list)
//		//{
//		//	rsrc_buf->second.Recycle(renderpass_manager);
//		//}
//
//		//for (const auto &rsrc_tex : pass->uni_tex_recycle_list)
//		//{
//		//	rsrc_tex->second.Recycle(renderpass_manager);
//		//}
//
//		////TODO: BARRIERS INSERSION after Pass？也许还需要在pass之后插入barrier？
//	}
//
//	bool RenderGraphV0::DependencyGraph::SortByTopologicallySortedPasses(const PassNode* a, const PassNode* b, const std::vector<PassNode*>& order)
//	{
//		auto it_a = std::find(order.begin(), order.end(), a);
//		auto it_b = std::find(order.begin(), order.end(), b);
//
//		// If both elements are found in the order vector, compare their positions
//		if (it_a != order.end() && it_b != order.end())
//		{
//			return std::distance(order.begin(), it_a) < std::distance(order.begin(), it_b);
//		}
//
//		// If only one element is found, prioritize it
//		if (it_a != order.end())
//		{
//			return true;
//		}
//		else if (it_b != order.end())
//		{
//			return false;
//		}
//		// If neither element is found, maintain the original order
//		return a < b;
//	}
//
//	bool RenderGraphV0::DependencyGraph::ExecutRenderGraphV0(VkCommandBuffer cmb)
//	{
//		//using namespace std;
//
//		////资源初始化信息 已经记录在attch_init_list，texture_init_list，buffer_init_list当中了，
//		////资源销毁信息 和 同步信息录入
//
//		////资源销毁信息录入
//		//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
//		//{
//		//	if (buf_itr->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//		//	{
//		//		const auto last_pass = FindLastAccessingPass(final_execution_order, buf_itr->second.passes_access_this_rsrc);
//		//		last_pass->buf_recycle_list.push_back(buf_itr);
//		//	}
//		//}
//
//		//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
//		//{
//		//	if (tex_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
//		//	{
//		//		const auto last_pass = FindLastAccessingPass(final_execution_order, tex_itr->second.passes_access_this_rsrc);
//		//		last_pass->tex_recycle_list.push_back(tex_itr);
//		//	}
//		//}
//
//		//for (auto attch_itr = attachments_map.begin(); attch_itr != attachments_map.end(); ++attch_itr)
//		//{
//		//	if (attch_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
//		//	{
//		//		const auto last_pass = FindLastAccessingPass(final_execution_order, attch_itr->second.passes_access_this_rsrc);
//		//		last_pass->attch_recycle_list.push_back(attch_itr);
//		//	}
//		//}
//
//		////同步信息录入这里sync info可以根据资源类型采取不同处理（资源的其他状态转换？比如sampler，view等，怎么处理？）
//
//		////BUF处理
//		//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
//		//{
//		//	SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
//		//	auto original_rsrc_name = buf_itr->second.name;
//
//		//	for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//		//	{
//		//		auto sec_pass_itr = std::next(fir_pass_itr);
//
//		//		//第一个使用的pass需要特殊处理
//		//		if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
//		//		{
//		//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
//		//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
//		//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
//		//			const auto fir_pass = *fir_pass_itr;
//
//		//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
//		//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");
//
//		//			auto outlet_itr = fir_pass->outs_buf.find(outlet_name);
//		//			if (outlet_itr != fir_pass->outs_buf.end())
//		//			{
//		//				auto rsrc_usage = outlet_itr->second.GetRsrcUsage();        //所有usages
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//target info (使用默认的)
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				//第一个使用方式拿出syncinfo
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//		//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second.GetItrInRsrcMap());
//		//			}
//
//		//			auto inlet_itr = fir_pass->ins_buf.find(inlet_name);
//		//			if (inlet_itr != fir_pass->ins_buf.end())
//		//			{
//		//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//target info
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second.GetItrInRsrcMap());
//		//			}
//		//		}
//
//		//		//如果第二个pass不是最后一个pass（存在至少两个pass）
//		//		if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
//		//		{
//		//			const auto fir_pass = *fir_pass_itr;
//		//			const auto sec_pass = *sec_pass_itr;
//
//		//			Vk::SyncInfo syn_info_source{};
//		//			Vk::SyncInfo syn_info_target{};
//
//		//			//同时有第二个pass和第一个pass
//		//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
//		//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");
//
//		//			if (fir_pass->outs_buf.contains(outlet_name_src_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->outs_buf.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//		//			if (fir_pass->ins_buf.contains(inlet_name_tar_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->ins_buf.find(inlet_name_tar_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//
//		//			//*******************************************************************
//		//			auto                                                                     outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
//		//			auto                                                                     inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
//		//			std::unordered_map<std::string, VirtualResource<Buffer>>::iterator vrsrc_itr;
//
//		//			if (sec_pass->outs_buf.contains(outlet_name_src_sec))
//		//			{
//		//				const auto  temp_itr       = sec_pass->outs_buf.find(outlet_name_src_sec);
//		//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//		//			}
//		//			if (sec_pass->ins_buf.contains(inlet_name_tar_sec))
//		//			{
//		//				const auto  temp_itr       = sec_pass->ins_buf.find(inlet_name_tar_sec);
//		//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//		//			}
//
//		//			//*****************************************************************************************
//		//			//RECORDING THE SYNC INFORMATION
//		//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//		//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
//		//		}
//		//	}
//		//}
//		////TEX处理
//		//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
//		//{
//		//	SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
//		//	auto original_rsrc_name = tex_itr->second.name;
//
//		//	for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//		//	{
//		//		auto sec_pass_itr = std::next(fir_pass_itr);
//
//		//		//第一个使用的pass需要特殊处理
//		//		if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
//		//		{
//		//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
//		//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
//		//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
//		//			const auto fir_pass = *fir_pass_itr;
//
//		//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
//		//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");
//
//		//			auto outlet_itr_tex = fir_pass->outs_tex.find(outlet_name);
//		//			if (outlet_itr_tex != fir_pass->outs_tex.end())
//		//			{
//		//				const auto &rsrc_usage = outlet_itr_tex->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//target info (使用默认的)
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				//从第一个使用方式拿出syncinfo
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());
//		//			}
//
//		//			auto outlet_itr_atta = fir_pass->outs_attach.find(outlet_name);
//		//			if (outlet_itr_atta != fir_pass->outs_attach.end())
//		//			{
//		//				const auto &rsrc_usage = outlet_itr_atta->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//target info (使用默认的)
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				//从第一个使用方式拿出syncinfo
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//		//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());
//		//			}
//
//		//			auto inlet_itr_tex = fir_pass->ins_tex.find(inlet_name);
//		//			if (inlet_itr_tex != fir_pass->ins_tex.end())
//		//			{
//		//				const auto &rsrc_usage = inlet_itr_tex->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//从第一个使用方式拿出syncinfo
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());
//		//			}
//
//		//			auto inlet_itr_atta = fir_pass->ins_tex.find(inlet_name);
//		//			if (inlet_itr_atta != fir_pass->ins_tex.end())
//		//			{
//		//				const auto &rsrc_usage = inlet_itr_atta->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//从第一个使用方式拿出syncinfo
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());
//		//			}
//		//		}
//
//		//		//如果第二个pass不是最后一个pass（存在至少两个pass）
//		//		if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
//		//		{
//		//			const auto fir_pass = *fir_pass_itr;
//		//			const auto sec_pass = *sec_pass_itr;
//
//		//			Vk::SyncInfo syn_info_source{};
//		//			Vk::SyncInfo syn_info_target{};
//
//		//			//同时有第二个pass和第一个pass
//		//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
//		//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");
//
//		//			if (fir_pass->outs_tex.contains(outlet_name_src_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//		//			if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->ins_tex.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//
//		//			if (fir_pass->outs_attach.contains(outlet_name_src_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//		//			if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->ins_attach.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//
//		//			//*******************************************************************
//		//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
//		//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
//		//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;
//
//		//			if (sec_pass->outs_tex.contains(outlet_name_src_sec))
//		//			{
//		//				const auto  temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
//		//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//		//			}
//		//			if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
//		//			{
//		//				const auto  temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
//		//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//		//			}
//
//		//			if (sec_pass->outs_attach.contains(outlet_name_src_sec))
//		//			{
//		//				const auto  temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
//		//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//		//			}
//		//			if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
//		//			{
//		//				const auto  temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
//		//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//		//			}
//
//		//			//*****************************************************************************************
//		//			//RECORDING THE SYNC INFORMATION
//		//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//		//			sec_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
//		//		}
//		//	}
//		//}
//
//		////ATTACHMENT处理   (同步信息+INFERENCE of the loading operation of ATTACHMENTs  推导attachment资源的 加载load 和 存储store 操作)
//		//for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
//		//{
//		//	SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
//		//	auto original_rsrc_name = attach_itr->second.name;
//
//		//	for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//		//	{
//		//		auto sec_pass_itr = std::next(fir_pass_itr);
//
//		//		//第一个使用的pass需要特殊处理
//		//		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
//		//		{
//		//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
//		//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
//		//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
//		//			const auto fir_pass = *fir_pass_itr;
//
//		//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
//		//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");
//
//		//			auto outlet_itr_atta = fir_pass->outs_attach.find(outlet_name);
//		//			if (outlet_itr_atta != fir_pass->outs_attach.end())
//		//			{
//		//				const auto rsrc_usage = outlet_itr_atta->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//target info (使用默认的)
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());
//
//		//				//因为是第一个使用的pass，目前attachment里面还没有内容
//		//				rsrc_usage->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		//				//因为使用的是Out函数导出，所以要用store operation
//		//				rsrc_usage->store_op = VK_ATTACHMENT_STORE_OP_STORE;
//		//			}
//
//		//			auto outlet_itr_tex = fir_pass->outs_tex.find(outlet_name);
//		//			if (outlet_itr_tex != fir_pass->outs_tex.end())
//		//			{
//		//				const auto rsrc_usage = outlet_itr_tex->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//target info (使用默认的)
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());
//
//		//				////因为是第一个使用的pass，目前attachment里面还没有内容
//		//				//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		//				////因为使用的是Out函数导出，所以要用store operation
//		//				//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
//		//			}
//
//		//			auto inlet_itr_atta = fir_pass->ins_attach.find(inlet_name);
//		//			if (inlet_itr_atta != fir_pass->ins_attach.end())
//		//			{
//		//				const auto rsrc_usage = inlet_itr_atta->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//target info
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());
//
//		//				//因为是第一个使用的pass，目前attachment里面还没有内容
//		//				rsrc_usage->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		//				//因为使用的是In函数，不会继续导出，所以要用Don't care
//		//				rsrc_usage->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		//			}
//
//		//			auto inlet_itr_tex = fir_pass->ins_tex.find(inlet_name);
//		//			if (inlet_itr_tex != fir_pass->ins_tex.end())
//		//			{
//		//				const auto rsrc_usage = inlet_itr_tex->second.GetRsrcUsage();
//		//				//*****************************************************************************************
//		//				//RECORDING THE SYNC INFORMATION
//		//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//		//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//		//				Vk::SyncInfo syn_info_source{
//		//				    .access_mask   = VK_ACCESS_NONE,
//		//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//		//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//		//				//target info
//		//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
//
//		//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());
//
//		//				//因为是第一个使用的pass，目前attachment里面还没有内容
//		//				//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		//				//因为使用的是In函数，不会继续导出，所以要用Don't care
//		//				//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		//			}
//		//		}
//
//		//		if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
//		//		{
//		//			const auto fir_pass = *fir_pass_itr;
//		//			const auto sec_pass = *sec_pass_itr;
//
//		//			Vk::SyncInfo syn_info_source{};
//		//			Vk::SyncInfo syn_info_target{};
//
//		//			//同时有第二个pass和第一个pass
//		//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
//		//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");
//
//		//			if (fir_pass->outs_attach.contains(outlet_name_src_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//		//			if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->ins_attach.find(inlet_name_tar_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//
//		//			if (fir_pass->outs_tex.contains(outlet_name_src_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//		//			if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
//		//			{
//		//				const auto temp_itr       = fir_pass->ins_tex.find(inlet_name_tar_fir);
//		//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//		//				syn_info_source           = rsrc_usage_src->GetSynInfo();
//		//			}
//
//		//			//*******************************************************************
//		//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
//		//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
//		//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;
//
//		//			if (sec_pass->outs_attach.contains(outlet_name_src_sec))
//		//			{
//		//				const auto temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
//		//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();
//
//		//				rsrc_usage_tar->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
//		//				rsrc_usage_tar->store_op = VK_ATTACHMENT_STORE_OP_STORE;
//		//			}
//		//			if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
//		//			{
//		//				const auto temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
//		//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();
//
//		//				//读写后不会导出，所以用dont care？存疑
//		//				rsrc_usage_tar->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
//		//				rsrc_usage_tar->store_op = VK_ATTACHMENT_STORE_OP_STORE;
//		//			}
//
//		//			if (sec_pass->outs_tex.contains(outlet_name_src_sec))
//		//			{
//		//				const auto temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
//		//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();
//
//		//				//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
//		//				//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
//		//			}
//		//			if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
//		//			{
//		//				const auto temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
//		//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//		//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
//		//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();
//
//		//				//读写后不会导出，所以用dont care？存疑
//		//				//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
//		//				//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
//		//			}
//
//		//			//*****************************************************************************************
//		//			//RECORDING THE SYNC INFORMATION
//		//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//		//			sec_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
//		//		}
//		//	}
//		//}
//
//		//PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)
//
//		//for (const auto pass_node_ptr : final_execution_order_hint)
//		//{
//		//	ExecutePass(cmb, pass_node_ptr);
//		//}
//
//		std::cout << "Render graph command recording finished" << std::endl;
//		return true;
//	}
//
//	bool RenderGraphV0::DependencyGraph::ParallelExecuteRenderGraphV0(const DeviceManager& device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore)
//	{
//		//std::vector<std::pair<RenderGraphV0::GraphicsPassNode *, DeviceManager::VkExecutionQueue *>> subgraph_queue_pairs;
//		//std::pair<RenderGraphV0::GraphicsPassNode *, DeviceManager::VkExecutionQueue *>              first_gfx_subgraph_queue_pair;
//		//bool                                                                                           first_gfx_subgraph_processed = false;
//
//		//std::unordered_map<RenderGraphV0::GraphicsPassNode *, DeviceManager::QueueCapability> subgraph_execute_on_queue_info;
//		//using OnQueueOrder = size_t;
//		//std::unordered_map<RenderGraphV0::GraphicsPassNode *, OnQueueOrder> subgraph_execute_on_queue_order;
//
//		//device_man.QueuesNumDisbatchedPassesToZero();
//
//		////Pre Execute
//		//for (auto &pass : gfx_pass_nodes)
//		//{
//		//	OnQueueOrder multi_queues_order = 0;
//		//	if (pass->is_rootpass)
//		//	{
//		//		multi_queues_order++;
//		//		VkRenderpassBaseRG::Type         acc_type = pass->GetAccumulatedType();
//		//		DeviceManager::QueueCapability queue_cap;
//		//		if (acc_type & VkRenderpassBaseRG::Type::Graphics)
//		//		{
//		//			queue_cap.graphics = true;
//		//		}
//
//		//		if (acc_type & VkRenderpassBaseRG::Type::Compute)
//		//		{
//		//			queue_cap.compute = true;
//		//		}
//
//		//		if (acc_type & VkRenderpassBaseRG::Type::Transfor)
//		//		{
//		//			queue_cap.transfer = true;
//		//		}
//
//		//		if (acc_type & VkRenderpassBaseRG::Type::Present)
//		//		{
//		//			queue_cap.present = true;
//		//		}
//
//		//		auto &target_queue = device_man.GetSuitableQueue(queue_cap);
//		//		target_queue.SetNumDisbatchedPasses(target_queue.GetNumDisbatchedPasses() + pass->subgraph_pass.size() + 1);
//
//		//		auto insersion_result_0 = subgraph_execute_on_queue_info.try_emplace(pass.get(), target_queue.GetQueueCap());
//		//		auto insersion_result_1 = subgraph_execute_on_queue_order.try_emplace(pass.get(), multi_queues_order);
//
//		//		if (!insersion_result_0.second || !insersion_result_1.second)
//		//		{
//		//			throw std::runtime_error("One subgraph can't be executed twice!");
//		//		}
//
//		//		//TODO: parallel recording cmd buf
//		//		//VkCommandBuffer cmd_buf = target_queue.GetOneNewBufFromCurQueue();
//		//		//pass->Execute(cmd_buf);
//		//		//for (auto subgraph_pass : pass->subgraph_pass)
//		//		//{
//		//		//	subgraph_pass->Execute(cmd_buf);
//		//		//}
//		//		//parallel recording done
//
//		//		//if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && first_gfx_subgraph_processed)
//		//		//{
//		//		//	target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf, image_available_semaphore);
//		//		//	first_gfx_subgraph_queue_pair.first  = pass.get();
//		//		//	first_gfx_subgraph_queue_pair.second = &target_queue;
//		//		//	first_gfx_subgraph_processed         = true;
//		//		//}
//		//		//else if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && !first_gfx_subgraph_processed)
//		//		//{
//		//		//	auto timeline_sem = first_gfx_subgraph_queue_pair.second->GetTimeLineSem();
//		//		//	target_queue.SubmitCmdBufThenSignalTimeLineSemThenWaitGfxTimeLineSem(cmd_buf, timeline_sem);
//
//		//		//	subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
//		//		//}
//		//		//else
//		//		//{
//		//		//	target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf);
//		//		//	subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
//		//		//}
//		//	}
//		//}
//
//		////DO IT AGAIN
//		//device_man.QueuesNumDisbatchedPassesToZero();
//		//for (auto &pass : gfx_pass_nodes)
//		//{
//		//	OnQueueOrder on_queue_order = 0;
//		//	if (pass->is_rootpass)
//		//	{
//		//		on_queue_order++;
//		//		VkRenderpassBaseRG::Type         acc_type = pass->GetAccumulatedType();
//		//		DeviceManager::QueueCapability queue_cap;
//		//		if (acc_type & VkRenderpassBaseRG::Type::Graphics)
//		//		{
//		//			queue_cap.graphics = true;
//		//		}
//
//		//		if (acc_type & VkRenderpassBaseRG::Type::Compute)
//		//		{
//		//			queue_cap.compute = true;
//		//		}
//
//		//		if (acc_type & VkRenderpassBaseRG::Type::Transfor)
//		//		{
//		//			queue_cap.transfer = true;
//		//		}
//
//		//		if (acc_type & VkRenderpassBaseRG::Type::Present)
//		//		{
//		//			queue_cap.present = true;
//		//		}
//		//		auto &target_queue = device_man.GetSuitableQueue(queue_cap);
//		//		target_queue.SetNumDisbatchedPasses(target_queue.GetNumDisbatchedPasses() + pass->subgraph_pass.size() + 1);
//
//		//		//TODO: PARALLEL RECORDING MULTI CMD BUF
//		//		VkCommandBuffer cmd_buf = target_queue.GetOneNewBufFromCurQueue();
//		//		pass->Execute(cmd_buf);
//		//		for (auto subgraph_pass : pass->subgraph_pass)
//		//		{
//		//			subgraph_pass->Execute(cmd_buf);
//		//		}
//		//		//PARALLEL RECORDING DONE
//
//		//		if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && first_gfx_subgraph_processed)
//		//		{
//		//			//target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf, image_available_semaphore);
//		//			first_gfx_subgraph_queue_pair.first  = pass.get();
//		//			first_gfx_subgraph_queue_pair.second = &target_queue;
//		//			first_gfx_subgraph_processed         = true;
//		//		}
//		//		else if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && !first_gfx_subgraph_processed)
//		//		{
//		//			auto timeline_sem = first_gfx_subgraph_queue_pair.second->GetTimeLineSem();
//		//			//target_queue.SubmitCmdBufThenSignalTimeLineSemThenWaitGfxTimeLineSem(cmd_buf, timeline_sem);
//
//		//			subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
//		//		}
//		//		else
//		//		{
//		//			//target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf);
//		//			subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
//		//		}
//		//	}
//		//}
//
//		//if (!first_gfx_subgraph_queue_pair.first || !first_gfx_subgraph_queue_pair.second)
//		//{
//		//	throw std::runtime_error("No graphics pass provided!");
//		//}
//
//		////harvests all Timeline Semaphores across all queues
//		//std::vector<VkSemaphore> all_waiting_queue_sems;
//		//for (auto &queue_pair : subgraph_queue_pairs)
//		//{
//		//	all_waiting_queue_sems.push_back(queue_pair.second->GetTimeLineSem());
//		//}
//
//		//first_gfx_subgraph_queue_pair.second->Finalize(all_waiting_queue_sems);
//
//		//device_man.QueuesTimeLineSemToZero();
//
//		//return true;
//	}
//
//	void RenderGraphV0::DependencyGraph::CmdBufExecuteOnQueue(PassNode* p_pass, VkCommandBuffer cmd_buf, DeviceManager::VkExecutionQueue* execution_queue, VkSemaphore image_available_semaphore)
//	{
//		//wait and signal semaphore
//
//		std::vector<VkSemaphoreSubmitInfo> wait_sem_submit_info;
//		std::vector<VkSemaphoreSubmitInfo> signal_sem_submit_info;
//
//		for (auto& diffq_head_sync : p_pass->buf_syn_infos_head_diff_q)
//		{
//			uint64_t wait_value = diffq_head_sync.wait_value.value_or(1);
//			wait_sem_submit_info.push_back(
//				VkSemaphoreSubmitInfo{
//					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//					.pNext = VK_NULL_HANDLE,
//					.semaphore = diffq_head_sync.sync_sema,
//					.value = wait_value,
//					.stageMask = diffq_head_sync.target_sync_info.stage_mask,
//					.deviceIndex = 0 });
//		}
//
//		for (auto& diffq_head_sync : p_pass->uni_tex_syn_infos_head_diff_q)
//		{
//			uint64_t wait_value = diffq_head_sync.wait_value.value_or(1);
//			wait_sem_submit_info.push_back(
//				VkSemaphoreSubmitInfo{
//					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//					.pNext = VK_NULL_HANDLE,
//					.semaphore = diffq_head_sync.sync_sema,
//					.value = wait_value,
//					.stageMask = diffq_head_sync.target_sync_info.stage_mask,
//					.deviceIndex = 0 });
//		}
//
//		for (auto& diffq_head_sync : p_pass->buf_syn_infos_tail_diff_q)
//		{
//			uint64_t signal_value = diffq_head_sync.signal_value.value_or(1);
//
//			signal_sem_submit_info.push_back(
//				VkSemaphoreSubmitInfo{
//					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//					.pNext = VK_NULL_HANDLE,
//					.semaphore = diffq_head_sync.sync_sema,
//					.value = signal_value,
//					.stageMask = diffq_head_sync.source_sync_info.stage_mask,
//					.deviceIndex = 0 });
//		}
//
//		for (auto& diffq_head_sync : p_pass->uni_tex_syn_infos_tail_diff_q)
//		{
//			uint64_t signal_value = diffq_head_sync.signal_value.value_or(1);
//
//			signal_sem_submit_info.push_back(
//				VkSemaphoreSubmitInfo{
//					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//					.pNext = VK_NULL_HANDLE,
//					.semaphore = diffq_head_sync.sync_sema,
//					.value = signal_value,
//					.stageMask = diffq_head_sync.source_sync_info.stage_mask,
//					.deviceIndex = 0 });
//		}
//
//		VkCommandBufferSubmitInfo cmd_submit_info{
//			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
//			.pNext = VK_NULL_HANDLE,
//			.commandBuffer = cmd_buf };
//
//		auto submit_info = VkSubmitInfo2{
//			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
//			.pNext = VK_NULL_HANDLE,
//			.flags = Vk::NoneFlag,
//			.waitSemaphoreInfoCount = static_cast<uint32_t>(wait_sem_submit_info.size()),
//			.pWaitSemaphoreInfos = wait_sem_submit_info.data(),
//			.commandBufferInfoCount = 1,
//			.pCommandBufferInfos = &cmd_submit_info,
//			.signalSemaphoreInfoCount = static_cast<uint32_t>(signal_sem_submit_info.size()),
//			.pSignalSemaphoreInfos = signal_sem_submit_info.data() };
//
//		vkQueueSubmit2(execution_queue->GetQueue(), 1, &submit_info, VK_NULL_HANDLE);
//	}
//
//	bool RenderGraphV0::DependencyGraph::ParallelExecuteRenderGraphV0V0(const DeviceManager& device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore)
//	{
//		//TODO: PARALLEL RECORDING MULTI CMD BUF. Should be done in a per-queue basis? You can't fill command buffers created by the same pool from multi threads
//
//		//先用简单的办法，一帧开始的时候初始化所有需要的资源？如果不这样，就需要先解决 多个pass同时读一份资源的初始化问题
//		//This is where transient resource should come in:
//		for (auto& ptr_pass : topologically_sorted_nodes)
//		{
//			ptr_pass->InitRsrc();
//		}
//
//		//在同一level中,pass的录制顺序并不重要，
//		for (auto level = 0; level <= max_level; ++level)
//		{
//			for (auto& ptr_pass : topologically_sorted_nodes)
//			{
//				if (ptr_pass->cur_level == level)
//				{
//					auto                                    ptr_queue = pass_to_queue_info.at(ptr_pass).first;
//					std::pair<VkCommandBuffer, VkSemaphore> cmd_buf_and_sem = ptr_queue->GetOneBufAndAssociatedSem();
//
//					//TODO: sampler and view change
//					ptr_pass->SyncInsertAhead(cmd_buf_and_sem.first);
//					ptr_pass->PreExecute();        //
//					ptr_pass->Execute(cmd_buf_and_sem.first);
//					ptr_pass->SyncInsertAfter(cmd_buf_and_sem.first);
//
//					CmdBufExecuteOnQueue(ptr_pass, cmd_buf_and_sem.first, ptr_queue, image_available_semaphore);
//				}
//			}
//		}
//
//		//Harvests all Timeline Semaphores across all queues
//		std::vector<VkSemaphore> all_waiting_queue_sems;
//
//		for (auto& [p_queue, _] : queue_to_passes_on_it)
//		{
//			for (auto& [_, sem] : p_queue->buf_to_sem)
//			{
//				all_waiting_queue_sems.push_back(sem);
//			}
//		}
//
//		DeviceManager::VkExecutionQueue* p_queue_used_to_finalize = nullptr;
//		for (auto& [p_queue, _] : queue_to_passes_on_it)
//		{
//			//TODO:这里找到queue cap最强的queue，然后用来finalize。暂时先随便选一个queue
//			if (!p_queue_used_to_finalize)
//			{
//				p_queue_used_to_finalize = p_queue;
//			}
//		}
//		p_queue_used_to_finalize->FinalizeGeneralRendering(all_waiting_queue_sems, general_rendering_finished_semaphore);
//
//		return true;
//	}
//
//	bool RenderGraphV0::DependencyGraph::Execute(VkCommandBuffer cmb)
//	{
//		using namespace std;
//
//		//资源初始化信息 已经记录在attch_init_list，texture_init_list，buffer_init_list当中了，
//		//资源销毁信息 和 同步信息录入
//
//		//资源销毁信息录入
//		for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
//		{
//			if (buf_itr->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//			{
//				const auto last_pass = FindLastAccessingPass(final_execution_order, buf_itr->second.passes_access_this_rsrc);
//				last_pass->buf_recycle_list.push_back(buf_itr);
//			}
//		}
//
//		for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
//		{
//			if (tex_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
//			{
//				const auto last_pass = FindLastAccessingPass(final_execution_order, tex_itr->second.passes_access_this_rsrc);
//				last_pass->tex_recycle_list.push_back(tex_itr);
//			}
//		}
//
//		for (auto attch_itr = attachments_map.begin(); attch_itr != attachments_map.end(); ++attch_itr)
//		{
//			if (attch_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
//			{
//				const auto last_pass = FindLastAccessingPass(final_execution_order, attch_itr->second.passes_access_this_rsrc);
//				last_pass->attch_recycle_list.push_back(attch_itr);
//			}
//		}
//
//		//for ( auto attach_itr = attachments_map.begin();attach_itr != attachments_map.end();++attach_itr)
//		//{
//		//	const auto original_rsrc_name = attach_itr->second.name;
//
//		//	for (auto fir_pass_itr =  attach_itr->second.passes_access_this_rsrc.begin();fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//		//	{
//		//		//  根据attach 会作为 inlet 输入pass：
//		//		//或 根据attach 会作为 outlet 输入pass：
//
//		//		const auto first_pass  = *fir_pass_itr;
//		//		auto outlet_name = original_rsrc_name + first_pass->name + std::string("Out");
//		//		auto inlet_name  = original_rsrc_name + first_pass->name + std::string("In");
//
//		//		//第一个pass特殊处理
//		//		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
//		//		{
//		//
//
//		//		}
//
//		//	}
//
//		//}
//
//
//		//同步信息录入这里sync info可以根据资源类型采取不同处理（资源状态转换？比如sampler，view等，怎么处理？）
//		//buf处理
//		for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
//		{
//			SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
//			auto original_rsrc_name = buf_itr->second.name;
//
//			for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//			{
//				auto sec_pass_itr = std::next(fir_pass_itr);
//
//				//第一个使用的pass需要特殊处理
//				if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
//				{
//					//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
//					//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
//					//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
//					const auto fir_pass = *fir_pass_itr;
//
//					auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
//					auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");
//
//					auto outlet_itr = fir_pass->outs_buf.find(outlet_name);
//					if (outlet_itr != fir_pass->outs_buf.end())
//					{
//						const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
//						//*****************************************************************************************
//						//RECORDING THE SYNC INFORMATION
//						//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//						//target info (使用默认的)
//						Vk::SyncInfo syn_info_source{
//						    .access_mask   = VK_ACCESS_NONE,
//						    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//						    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//						//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//						Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//
//						fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
//					}
//
//					auto inlet_itr = fir_pass->ins_buf.find(inlet_name);
//					if (inlet_itr != fir_pass->ins_buf.end())
//					{
//						const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
//						//*****************************************************************************************
//						//RECORDING THE SYNC INFORMATION
//						//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//						//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//						Vk::SyncInfo syn_info_source{
//						    .access_mask   = VK_ACCESS_NONE,
//						    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//						    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//						//target info
//						Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//
//						fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
//					}
//				}
//
//				if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
//				{
//					const auto fir_pass = *fir_pass_itr;
//					const auto sec_pass = *sec_pass_itr;
//
//					Vk::SyncInfo syn_info_source{};
//					Vk::SyncInfo syn_info_target{};
//
//					//同时有第二个pass和第一个pass
//					auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
//					auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");
//
//					if (fir_pass->outs_buf.contains(outlet_name_src_fir))
//					{
//						const auto  temp_itr       = fir_pass->outs_buf.find(outlet_name_src_fir);
//						const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//						syn_info_source            = rsrc_usage_src.GetSynInfo();
//					}
//					if (fir_pass->ins_buf.contains(inlet_name_tar_fir))
//					{
//						const auto  temp_itr       = fir_pass->ins_buf.find(inlet_name_tar_fir);
//						const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//						syn_info_source            = rsrc_usage_src.GetSynInfo();
//					}
//
//					//*******************************************************************
//					auto                                                                     outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
//					auto                                                                     inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
//					std::unordered_map<std::string, VirtualResource<Buffer>>::iterator vrsrc_itr;
//
//					if (sec_pass->outs_buf.contains(outlet_name_src_sec))
//					{
//						const auto  temp_itr       = sec_pass->outs_buf.find(outlet_name_src_sec);
//						const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//						syn_info_target            = rsrc_usage_tar.GetSynInfo();
//						vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//					}
//					if (sec_pass->ins_buf.contains(inlet_name_tar_sec))
//					{
//						const auto  temp_itr       = sec_pass->ins_buf.find(inlet_name_tar_sec);
//						const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//						syn_info_target            = rsrc_usage_tar.GetSynInfo();
//						vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//					}
//
//					//*****************************************************************************************
//					//RECORDING THE SYNC INFORMATION
//					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//					sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
//				}
//			}
//		}
//		//tex处理
//		for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
//		{
//			SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
//			auto original_rsrc_name = tex_itr->second.name;
//
//			for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//			{
//				auto sec_pass_itr = std::next(fir_pass_itr);
//
//				//第一个使用的pass需要特殊处理
//				if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
//				{
//					//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
//					//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
//					//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
//					const auto fir_pass = *fir_pass_itr;
//
//					auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
//					auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");
//
//					auto outlet_itr = fir_pass->outs_tex.find(outlet_name);
//					if (outlet_itr != fir_pass->outs_tex.end())
//					{
//						const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
//						//*****************************************************************************************
//						//RECORDING THE SYNC INFORMATION
//						//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//						//target info (使用默认的)
//						Vk::SyncInfo syn_info_source{
//						    .access_mask   = VK_ACCESS_NONE,
//						    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//						    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//						//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//						Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//
//						fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
//					}
//
//					auto inlet_itr = fir_pass->ins_tex.find(inlet_name);
//					if (inlet_itr != fir_pass->ins_tex.end())
//					{
//						const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
//						//*****************************************************************************************
//						//RECORDING THE SYNC INFORMATION
//						//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//						//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//						Vk::SyncInfo syn_info_source{
//						    .access_mask   = VK_ACCESS_NONE,
//						    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//						    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//						//target info
//						Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//
//						fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
//					}
//				}
//
//				if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
//				{
//					const auto fir_pass = *fir_pass_itr;
//					const auto sec_pass = *sec_pass_itr;
//
//					Vk::SyncInfo syn_info_source{};
//					Vk::SyncInfo syn_info_target{};
//
//					//同时有第二个pass和第一个pass
//					auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
//					auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");
//
//					if (fir_pass->outs_tex.contains(outlet_name_src_fir))
//					{
//						const auto  temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
//						const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//						syn_info_source            = rsrc_usage_src.GetSynInfo();
//					}
//					if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
//					{
//						const auto  temp_itr       = fir_pass->ins_tex.find(inlet_name_tar_fir);
//						const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//						syn_info_source            = rsrc_usage_src.GetSynInfo();
//					}
//
//					//*******************************************************************
//					auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
//					auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
//					std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;
//
//					if (sec_pass->outs_tex.contains(outlet_name_src_sec))
//					{
//						const auto  temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
//						const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//						syn_info_target            = rsrc_usage_tar.GetSynInfo();
//						vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//					}
//					if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
//					{
//						const auto  temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
//						const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//						syn_info_target            = rsrc_usage_tar.GetSynInfo();
//						vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
//					}
//
//					//*****************************************************************************************
//					//RECORDING THE SYNC INFORMATION
//					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//					sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
//				}
//			}
//		}
//		//attachmen处理(同步信息+INFERENCE of the loading operation of ATTACHMENTs  推导attachment资源的 加载load 和 存储store 操作)
//		for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
//		{
//			SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
//			auto original_rsrc_name = attach_itr->second.name;
//
//			for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
//			{
//				auto sec_pass_itr = std::next(fir_pass_itr);
//
//				//第一个使用的pass需要特殊处理
//				if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
//				{
//					//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
//					//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
//					//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
//					const auto fir_pass = *fir_pass_itr;
//
//					auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
//					auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");
//
//					auto outlet_itr = fir_pass->outs_attach.find(outlet_name);
//					if (outlet_itr != fir_pass->outs_attach.end())
//					{
//						auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
//						//*****************************************************************************************
//						//RECORDING THE SYNC INFORMATION
//						//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//						//target info (使用默认的)
//						Vk::SyncInfo syn_info_source{
//						    .access_mask   = VK_ACCESS_NONE,
//						    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//						    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//						//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//						Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//
//						fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
//
//						//因为是第一个使用的pass，目前attachment里面还没有内容
//						rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//						//因为使用的是Out函数导出，所以要用store operation
//						rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_STORE;
//					}
//
//					auto inlet_itr = fir_pass->ins_attach.find(inlet_name);
//					if (inlet_itr != fir_pass->ins_attach.end())
//					{
//						auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
//						//*****************************************************************************************
//						//RECORDING THE SYNC INFORMATION
//						//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//
//						//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
//						Vk::SyncInfo syn_info_source{
//						    .access_mask   = VK_ACCESS_NONE,
//						    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//						    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
//
//						//target info
//						Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
//
//						fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
//
//						//因为是第一个使用的pass，目前attachment里面还没有内容
//						rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//						//因为使用的是In函数，不会导出，所以要用Don't care
//						rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//					}
//				}
//
//				if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
//				{
//					const auto fir_pass = *fir_pass_itr;
//					const auto sec_pass = *sec_pass_itr;
//
//					Vk::SyncInfo syn_info_source{};
//					Vk::SyncInfo syn_info_target{};
//
//					//同时有第二个pass和第一个pass
//					auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
//					auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");
//
//					if (fir_pass->outs_attach.contains(outlet_name_src_fir))
//					{
//						const auto  temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
//						const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//						syn_info_source            = rsrc_usage_src.GetSynInfo();
//					}
//					if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
//					{
//						const auto  temp_itr       = fir_pass->ins_attach.find(inlet_name_tar_fir);
//						const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
//						syn_info_source            = rsrc_usage_src.GetSynInfo();
//					}
//
//					//*******************************************************************
//					auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
//					auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
//					std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;
//
//					if (sec_pass->outs_attach.contains(outlet_name_src_sec))
//					{
//						const auto temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
//						auto &     rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//						syn_info_target           = rsrc_usage_tar.GetSynInfo();
//						vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();
//
//						rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
//						rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
//					}
//					if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
//					{
//						const auto temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
//						auto &     rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
//						syn_info_target           = rsrc_usage_tar.GetSynInfo();
//						vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();
//
//						//读写后不会导出，所以用dont care？存疑
//						rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
//						rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
//					}
//
//					//*****************************************************************************************
//					//RECORDING THE SYNC INFORMATION
//					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
//					sec_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
//				}
//			}
//		}
//
//		//PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)
//
//		for (const auto pass_node_ptr : final_execution_order)
//		{
//			ExecutePass(cmb, pass_node_ptr);
//		}
//
//		std::cout << "Render graph command recording finished" << std::endl;
//		return true;
//	}
//
//	void RenderGraphV0::DependencyGraph::GeneratePassNodeDepenFromBuf(PassNode& pass_node)
//	{
//		for (auto& [_, inlet] : pass_node.ins_buf)
//		{
//			//在当前pass中的使用方式usage
//			const auto usage = inlet.GetRsrcUsage();
//			auto& buf_usage = *usage;
//
//			//如果使用了读的方式，则
//			if (buf_usage.GetRsrcAccessType() == Vk::AccessType::Read)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<Buffer, VkBufUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                  providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					//providing_outlet_itor一定是outlet
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
//					{
//						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
//						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//						//{
//						//	gfx_node->passes_depen_set.insert(previous_pass);
//						//}
//
//						break;
//					}
//				}
//			}
//			else if (buf_usage.GetRsrcAccessType() == Vk::AccessType::Write)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<Buffer, VkBufUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                  providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
//					{
//						if (inlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//						{
//							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
//							if (previous_pass != &pass_node)
//							{
//								pass_node.passes_depen_set.insert(previous_pass);
//							}
//						}
//						break;
//					}
//				}
//			}
//		}
//
//		for (auto& [_, outlet] : pass_node.outs_buf)
//		{
//			//在当前pass中的使用方式usage
//			const auto usage = outlet.GetRsrcUsage();
//			auto& buf_usage = *usage;
//
//			//如果使用了读的方式，则
//			if (buf_usage.GetRsrcAccessType() == Vk::AccessType::Read)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<Buffer, VkBufUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                  providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					//providing_outlet_itor一定是outlet
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
//					{
//						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
//						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//						//{
//						//	gfx_node->passes_depen_set.insert(previous_pass);
//						//}
//
//						break;
//					}
//				}
//			}
//			else if (buf_usage.GetRsrcAccessType() == Vk::AccessType::Write)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<Buffer, VkBufUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                  providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
//					{
//						if (outlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//						{
//							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
//							if (previous_pass != &pass_node)
//							{
//								pass_node.passes_depen_set.insert(previous_pass);
//							}
//						}
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	void RenderGraphV0::DependencyGraph::GeneratePassNodeDepenFromTex(PassNode& pass_node)
//	{
//		for (auto& [_, inlet] : pass_node.ins_uni_tex)
//		{
//			const auto usage = inlet.GetRsrcUsage();
//			auto& tex_usage = *usage;
//
//			if (tex_usage.GetRsrcAccessType() == Vk::AccessType::Read)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                        providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					//providing_outlet_itor一定是outlet
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
//					{
//						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
//						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//						//{
//						//	gfx_node->passes_depen_set.insert(previous_pass);
//						//}
//
//						break;
//					}
//				}
//			}
//
//			else if (tex_usage.GetRsrcAccessType() == Vk::AccessType::Write)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                        providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
//					{
//						if (inlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
//						{
//							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
//							if (previous_pass != &pass_node)
//							{
//								pass_node.passes_depen_set.insert(previous_pass);
//							}
//						}
//						break;
//					}
//				}
//			}
//		}
//
//		for (auto& [_, outlet] : pass_node.outs_uni_tex)
//		{
//			const auto usage = outlet.GetRsrcUsage();
//			auto& tex_usage = *usage;
//
//			if (tex_usage.GetRsrcAccessType() == Vk::AccessType::Read)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                        providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					//providing_outlet_itor一定是outlet
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
//					{
//						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
//						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<Buffer>::RsrcType::Established)
//						//{
//						//	gfx_node->passes_depen_set.insert(previous_pass);
//						//}
//
//						break;
//					}
//				}
//			}
//
//			else if (tex_usage.GetRsrcAccessType() == Vk::AccessType::Write)
//			{
//				PassNode* previous_pass = &pass_node;
//				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
//				auto                                                                                        providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();
//
//				while (true)
//				{
//					if (providing_pass_and_source_outlet_itr)
//					{
//						auto providing_itr = providing_pass_and_source_outlet_itr->second;
//						auto providing_pass = providing_pass_and_source_outlet_itr->first;
//
//						previous_pass = providing_pass;
//						previous_out_itr = providing_itr;
//
//						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetRsrcAccessType();
//
//						if (providing_pass_access_type == Vk::AccessType::Read)
//						{
//							pass_node.passes_depen_set.insert(providing_pass);
//							break;
//						}
//						if (providing_pass_access_type == Vk::AccessType::Write)
//						{
//							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
//						}
//					}
//					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
//					{
//						if (outlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
//						{
//							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
//							if (previous_pass != &pass_node)
//							{
//								pass_node.passes_depen_set.insert(previous_pass);
//							}
//						}
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	void RenderGraphV0::DependencyGraph::GeneratePassNodeDepen()
//	{
//		//DEPENDENCY GENERATION
//		for (const auto& gfx_node : gfx_pass_nodes)
//		{
//			//对于每一个inlet，会对其中的资源进行读或者写
//			//case 0:如果是对这个资源进行了读取，则要去链上寻找前一个写入该资源的pass
//			//case 1:如果是对这个资源进行了写入，则要去链上寻找前一个读取该资源的pass（因为会写入会暴力覆盖之前pass中写入的内容）
//			//TODO: 多线程优化
//			GeneratePassNodeDepenFromBuf(*gfx_node);
//			GeneratePassNodeDepenFromTex(*gfx_node);
//			std::remove(gfx_node->passes_depen_set.begin(), gfx_node->passes_depen_set.end(), nullptr);
//		}
//
//		//生成每个pass之后才能执行的pass 和 每个pass之前必须先执行的pass
//		for (const auto& gfx_node : gfx_pass_nodes)
//		{
//			for (const auto& ptr_before_pass : gfx_node->passes_depen_set)
//			{
//				ptr_before_pass->passes_depen_on_cur_pass_set.insert(gfx_node.get());
//			}
//		}
//	}
//
//	void RenderGraphV0::DependencyGraph::VisitNode(size_t cur_index, PassNode* cur_node_pass, std::vector<bool>& visited, std::vector<bool>& on_stack, std::vector<PassNode*>& topologically_sorted_nodes)
//	{
//		if (visited[cur_index] && on_stack[cur_index])
//		{
//			throw std::runtime_error("found a circular dependency and must abort.");
//		}
//		if (visited[cur_index])
//		{
//			return;
//		}
//
//		visited[cur_index] = true;
//		on_stack[cur_index] = true;
//
//		for (PassNode* adj_list : cur_node_pass->passes_depen_on_cur_pass_set)
//		{
//			VisitNode(FindIndexInPassNodeArray(adj_list, gfx_pass_nodes), adj_list, visited, on_stack, topologically_sorted_nodes);
//		}
//
//		topologically_sorted_nodes.push_back(cur_node_pass);
//		on_stack[cur_index] = false;
//	}
//
//	size_t RenderGraphV0::DependencyGraph::FindIndexInPassNodeArray(PassNode* node_pass, const std::vector<std::unique_ptr<GraphicsPassNode>>& all_nodes)
//	{
//		for (auto gfx_node_itr = all_nodes.begin(); gfx_node_itr < all_nodes.end(); ++gfx_node_itr)
//		{
//			if (gfx_node_itr->get() == node_pass)
//			{
//				return std::distance(all_nodes.begin(), gfx_node_itr);
//			}
//		}
//		throw std::runtime_error("pass not found!");
//	}
//
//	void RenderGraphV0::DependencyGraph::TopologicalDfsSort()
//	{
//		//拓扑排序结合dfs算法，划分层次
//		std::vector<bool> visited(gfx_pass_nodes.size(), false);
//		std::vector<bool> on_stack(gfx_pass_nodes.size(), false);
//
//		for (auto gfx_node_itr = gfx_pass_nodes.begin(); gfx_node_itr < gfx_pass_nodes.end(); ++gfx_node_itr)
//		{
//			auto index = std::distance(gfx_pass_nodes.begin(), gfx_node_itr);
//			VisitNode(index, gfx_node_itr->get(), visited, on_stack, topologically_sorted_nodes);
//
//			if (topologically_sorted_nodes.size() == gfx_pass_nodes.size())
//			{
//				break;
//			}
//		}
//		std::ranges::reverse(topologically_sorted_nodes);
//	}
//	void RenderGraphV0::DependencyGraph::SortPassesAccessingRsrc()
//	{
//		//对每个资源的使用者（passes）进行排序
//		for (auto& [_, vrsc] : buffers_map)
//		{
//			// Sorting the unordered vector based on the order of the order vector(that is, topologically_sorted_nodes)
//			std::sort(vrsc.passes_access_this_rsrc.begin(), vrsc.passes_access_this_rsrc.end(),
//				[&](const std::pair<RenderGraphV0::PassNode*, Vk::AccessType>& pair_left, const std::pair<RenderGraphV0::PassNode*, Vk::AccessType>& pair_right)
//				{
//					return SortByTopologicallySortedPasses(pair_left.first, pair_right.first, topologically_sorted_nodes);
//				});
//		}
//
//		for (const auto& [_, vrsc] : uni_textures_map)
//		{
//			// Sorting the unordered vector based on the order of the order vector
//			std::sort(vrsc.passes_access_this_rsrc.begin(), vrsc.passes_access_this_rsrc.end(),
//				[&](const std::pair<RenderGraphV0::PassNode*, Vk::AccessType>& pair_left, const std::pair<RenderGraphV0::PassNode*, Vk::AccessType>& pair_right)
//				{
//					return SortByTopologicallySortedPasses(pair_left.first, pair_right.first, topologically_sorted_nodes);
//				});
//		}
//	}
//
//	void RenderGraphV0::DependencyGraph::DependencyLevelsGeneration()
//	{
//		///************************************************************
//		//DEPENDENCY LEVELS(LAYERS) GENERATION层次划分:
//		///************************************************************
//		std::vector<size_t> cur_level(topologically_sorted_nodes.size(), 0);
//		for (auto pas_itr = topologically_sorted_nodes.begin(); pas_itr < topologically_sorted_nodes.end(); ++pas_itr)
//		{
//			size_t cur_index = std::distance(topologically_sorted_nodes.begin(), pas_itr);
//			for (auto adjacent_pass : (*pas_itr)->passes_depen_on_cur_pass_set)
//			{
//				// Use std::find_if with a lambda function
//				auto it_adj_pass = std::ranges::find_if(topologically_sorted_nodes,
//					[adjacent_pass](const PassNode* node_itr)
//					{
//						return node_itr == adjacent_pass;
//					});
//				if (it_adj_pass == topologically_sorted_nodes.end())
//				{
//					throw std::runtime_error("no pass found!");
//				}
//
//				size_t adj_index = std::distance(topologically_sorted_nodes.begin(), it_adj_pass);
//				if (cur_level[adj_index] < cur_level[cur_index] + 1)
//				{
//					cur_level[adj_index] = cur_level[cur_index] + 1;
//				}
//			}
//		}
//
//		for (auto i = 0; i < topologically_sorted_nodes.size(); ++i)
//		{
//			topologically_sorted_nodes[i]->cur_level = cur_level[i];
//		}
//		const auto max_level_itr = std::ranges::max_element(cur_level);
//		max_level = *max_level_itr;
//	}
//
//	void RenderGraphV0::DependencyGraph::GPUWorkDistribution()
//	{
//		///************************************************************
//		//DISTRIBUTE WORK TO MULTIPLE QUEUES ONTO GPU:
//		///************************************************************
//		//std::unordered_map<DeviceManager::VkExecutionQueue *, std::list<PassNode *>>         queue_to_passes_on_it;
//		//std::unordered_map<PassNode *, std::pair<DeviceManager::VkExecutionQueue *, size_t>> pass_to_queue_info;
//
//		device_man.QueuesNumDisbatchedPassesToZero();
//		size_t monotonical_assigned_index = 1;
//		for (auto level = 0; level <= max_level; ++level)
//		{
//			for (auto& ptr_pass : topologically_sorted_nodes)
//			{
//				if (ptr_pass->cur_level == level)
//				{
//					VkRenderpassBaseRG::Type         pass_type = ptr_pass->GetRenderpassType();
//					DeviceManager::QueueCapability queue_cap_requirement;
//					if (pass_type & VkRenderpassBaseRG::Type::Graphics)
//					{
//						queue_cap_requirement.graphics = true;
//					}
//
//					if (pass_type & VkRenderpassBaseRG::Type::Compute)
//					{
//						queue_cap_requirement.compute = true;
//					}
//
//					if (pass_type & VkRenderpassBaseRG::Type::Transfor)
//					{
//						queue_cap_requirement.transfer = true;
//					}
//
//					if (pass_type & VkRenderpassBaseRG::Type::Present)
//					{
//						queue_cap_requirement.present = true;
//					}
//					auto& target_queue = device_man.GetSuitableQueue2(queue_cap_requirement);
//					auto  ptr_queue = &target_queue;
//					if (queue_to_passes_on_it.contains(ptr_queue))
//					{
//						queue_to_passes_on_it.at(ptr_queue).push_back(ptr_pass);
//					}
//					else
//					{
//						queue_to_passes_on_it.emplace(ptr_queue, std::list<PassNode*>{ptr_pass});
//					}
//
//					auto result_itr = pass_to_queue_info.emplace(ptr_queue, std::pair<DeviceManager::VkExecutionQueue*, size_t>{ptr_queue, monotonical_assigned_index});
//
//					if (!result_itr.second)
//					{
//						throw std::runtime_error("the pass has been distributed to queue!");
//					}
//					monotonical_assigned_index++;
//				}
//			}
//		}
//
//		std::vector<DeviceManager::VkExecutionQueue*> all_queue_in_use;        //确定一个queue的顺序，方便之后用index索引
//
//		// Extract keys from the map and store them in the vector
//		std::ranges::transform(queue_to_passes_on_it, std::back_inserter(all_queue_in_use),
//			[](const std::pair<DeviceManager::VkExecutionQueue*, std::list<PassNode*>>& pair)
//			{
//				return pair.first;        // Extract the key
//			});
//	}
//
//	void RenderGraphV0::DependencyGraph::SyncPrimitivesAquisition()
//	{
//		//TODO:用同步原语工厂产生需要的同步原语
//		VkEvent     shit_event;
//		VkSemaphore shit_sema;
//		VkSemaphore shit_sema1;
//
//		for (auto itr = topologically_sorted_nodes.begin(); std::next(itr) != topologically_sorted_nodes.end(); ++itr)
//		{
//			same_queue_sync_event.emplace(PassPair{ *itr, *std::next(itr) }, shit_event);
//			diff_queue_sync_sema.emplace(PassPair{ *itr, *std::next(itr) }, shit_sema);
//		}
//
//		for (size_t level = 0; level <= max_level; ++level)
//		{
//			same_level_sync_sema.emplace(level, shit_sema1);
//		}
//	}
//
//	void RenderGraphV0::DependencyGraph::SortPassesAtSameLevel()
//	{
//		///************************************************************
//		//同level的所有passes排序:
//		///************************************************************
//
//		//首先对于所有的资源，所有使用到资源的pass按照layer排序以后，每一个layer层级内的pass都需要进行排序，否则这个执行顺序定不下来，根本写不了同步
//		for (auto& [_, vrsrc] : buffers_map)
//		{
//			for (auto level = 0; level <= max_level; ++level)
//			{
//				vrsrc.level_to_passes_attached_to.emplace(level, std::vector<RenderGraphV0::PassNode*>{});
//				vrsrc.level_to_unique_queues.emplace(level, std::unordered_set<DeviceManager::VkExecutionQueue*>{});
//			}
//
//			//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
//			std::ranges::for_each(
//				topologically_sorted_nodes, [&vrsrc](PassNode* const cur_pass)
//				{
//					vrsrc.level_to_passes_attached_to.at(cur_pass->cur_level).push_back(cur_pass);
//				});
//
//			//必须把这些同layer的passes按照他们执行的queue进行分块
//			for (auto& [level, passes_at_same_levels] : vrsrc.level_to_passes_attached_to)
//			{
//				for (auto& pass : passes_at_same_levels)
//				{
//					vrsrc.level_to_unique_queues.at(level).insert(pass_to_queue_info.at(pass).first);
//				}
//
//				std::vector<std::vector<RenderGraphV0::PassNode*>> partitions_by_queue;
//
//				for (auto unique_queue : vrsrc.level_to_unique_queues.at(level))        //unique queue为指针
//				{
//					std::vector<RenderGraphV0::PassNode*> partition;
//					std::copy_if(passes_at_same_levels.begin(), passes_at_same_levels.end(), std::back_inserter(partition),
//						[this, unique_queue](RenderGraphV0::PassNode* pass) -> bool
//						{
//							return
//
//								this->pass_to_queue_info.at(pass).first == unique_queue;
//						});
//					partitions_by_queue.push_back(std::move(partition));
//				}
//
//				passes_at_same_levels.clear();
//
//				for (const auto& inner_partition : partitions_by_queue)
//				{
//					for (auto element : inner_partition)
//					{
//						passes_at_same_levels.push_back(element);
//					}
//				}
//			}
//		}
//	}
//
//	void RenderGraphV0::DependencyGraph::SyncPrimitivesInsertion()
//	{
//		///************************************************************
//		//SYNC PRIMITIVES INSERTION同步原语插入:
//		///************************************************************
//		//按照level层次来loop，
//		for (auto cur_level = 0; cur_level <= max_level; ++cur_level)
//		{
//			std::ranges::for_each(
//				topologically_sorted_nodes,
//				[cur_level, this](PassNode* const cur_pass)
//				{
//					if (cur_pass->cur_level == cur_level)        //处理顺序：level0，level1，level2 ... ...
//					{
//						//TODO:如何处理TAA资源
//
//						//处理inlet buf
//						for (auto& [_, cur_in_let] : cur_pass->ins_buf)
//						{
//							auto& underlying_rsrc = cur_in_let.GetUnderlyingRsrcItr()->second;
//							auto& usage = *cur_in_let.GetRsrcUsage();
//
//							//得到资源的源头
//							const auto origin = usage.GetRsrcOrigin();
//
//							//直接在当前pass建立的资源，没什么好说的，同步原语插入很简单：
//							if (RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu == origin)
//							{
//								auto source_sync = Vk::BufSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.offset = 0,
//									.size = VK_WHOLE_SIZE };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_in_let.GetRsrcUsage()->GetBufSynInfo(),
//									cur_in_let.GetUnderlyingRsrcItr());
//								continue;
//							}
//
//							//拿到所有使用该资源的pass，对于之前的每一个layer，进行同步原语插入。比如当前layer是5，我们首先需要找到layer=1,2,3,4的所有pass，然后对这些layer中的所有pass最后一个使用的pass和当前pass之间插入barrier
//							for (auto& [level, passes_at_same_levels] : cur_in_let.GetUnderlyingRsrcItr()->second.level_to_passes_attached_to)
//							{
//								if (level < cur_level)
//								{
//									//找到对应level中最后一个使用者，插入同步原语
//									auto sor_pass = passes_at_same_levels.back();
//									auto tar_pass = cur_pass;
//
//									auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
//									auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
//
//									auto sor_pass_queue_cap = sor_pass_queue_ptr->GetQueueCap();
//									auto tar_pass_queue_cap = tar_pass_queue_ptr->GetQueueCap();
//
//									VkBufUsageInfoRG* sor_buf_usage = nullptr;
//									for (auto& usage : sor_pass->buf_usages)
//									{
//										if (&(usage.buffer->second) == &(cur_in_let.GetUnderlyingRsrcItr()->second))
//										{
//											sor_buf_usage = usage.buf_usage.get();
//										}
//									}
//									if (!sor_buf_usage)
//									{
//										throw std::runtime_error("no resource found!");
//									}
//
//									auto& target_let = cur_in_let;
//
//									//Cur pass     ------------------------->       Tar pass
//									//Sync primitive
//									auto sync_event = same_queue_sync_event.at(PassPair{ sor_pass, tar_pass });
//									auto sync_sema = diff_queue_sync_sema.at(PassPair{ sor_pass, tar_pass });
//
//									//CASE 1: in a same queue situation
//									if (sor_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index)
//									{
//										//because we use barrier, so only one barrier is needed.
//										tar_pass->InsertSameQueueSyncInfo(
//											sor_pass,
//											tar_pass,
//											sync_event,
//											tar_pass_queue_cap,
//											sor_buf_usage->GetBufSynInfo(),
//											target_let.GetRsrcUsage()->GetBufSynInfo(),
//											target_let.GetUnderlyingRsrcItr());
//									}
//									//CASE 2: DIFF queue situation (need queue ownership transfer!)
//									else
//									{
//										sor_pass->InsertDiffQueueSyncInfoBinarySem(
//											sor_pass,
//											tar_pass,
//											sync_sema,
//											sor_pass_queue_ptr,
//											tar_pass_queue_ptr,
//											sor_buf_usage->GetBufSynInfo(),
//											target_let.GetRsrcUsage()->GetBufSynInfo(),
//											target_let.GetUnderlyingRsrcItr());
//
//										tar_pass->InsertDiffQueueSyncInfoBinarySem(
//											sor_pass,
//											tar_pass,
//											sync_sema,
//											sor_pass_queue_ptr,
//											tar_pass_queue_ptr,
//											sor_buf_usage->GetBufSynInfo(),
//											target_let.GetRsrcUsage()->GetBufSynInfo(),
//											target_let.GetUnderlyingRsrcItr());
//									}
//								}
//
//								//同时，对于当前同一个layer的所有pass，我们要定一个执行顺序，
//								if (level == cur_level)
//								{
//									//passes_at_same_levels排布一定是 按照queue 分块的
//									// 比如passes_at_same_levels是   pass0 , pass1 , pass4, pass5 , pass7 , pass9
//									// 块0 , queue3 ：|pass0，pass1，pass4|  块1 , queue 2 ：|pass5，pass7，pass9|
//
//									//在同一个level中决定如何同步：在同一level中的pass应该同时读一个资源，这种情况只需要处理不同queue的queue ownership transfer的问题?
//
//									auto sync_timeline_sema_same_level = same_level_sync_sema.at(level);
//
//									auto& unique_queues_set = (cur_in_let.GetUnderlyingRsrcItr()->second).level_to_unique_queues.at(level);
//									auto  cur_pass_pos_in_cur_level = std::find(passes_at_same_levels.begin(), passes_at_same_levels.end(), cur_pass);
//									auto  cur_queue_itr = unique_queues_set.find(pass_to_queue_info.at(cur_pass).first);
//
//									auto sor_pass = cur_pass;
//									auto tar_pass = cur_pass;
//
//									if (cur_pass_pos_in_cur_level != passes_at_same_levels.begin())
//									{
//										// Get the iterator to the previous element
//										auto previousElementIterator = std::prev(cur_pass_pos_in_cur_level);
//
//										// Access the previous element
//										sor_pass = *previousElementIterator;
//
//										if (cur_queue_itr != unique_queues_set.end())
//										{
//											// Element found in the set
//											std::size_t index = std::distance(unique_queues_set.begin(), cur_queue_itr);
//											uint64_t    wait_value = index;
//											uint64_t    signal_value = index + 1;
//
//											auto sor_pass_queue_ptr = pass_to_queue_info.at(sor_pass).first;
//											auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
//
//											auto sor_pass_queue_cap = sor_pass_queue_ptr->GetQueueCap();
//											auto tar_pass_queue_cap = tar_pass_queue_ptr->GetQueueCap();
//
//											VkBufUsageInfoRG* sor_buf_usage = nullptr;
//											for (auto& usage : sor_pass->buf_usages)
//											{
//												if (&(usage.buffer->second) == &(cur_in_let.GetUnderlyingRsrcItr()->second))
//												{
//													sor_buf_usage = usage.buf_usage.get();
//												}
//											}
//											if (!sor_buf_usage)
//											{
//												throw std::runtime_error("no resource found!");
//											}
//
//											auto& target_let = cur_in_let;
//
//											//Source pass     ------------------------->       Cur pass
//											//Sync primitive
//											auto sync_event = same_queue_sync_event.at(PassPair{ sor_pass, tar_pass });
//											//auto sync_sema  = diff_queue_sync_sema.at(PassPair{sor_pass, tar_pass});
//
//											//CASE 1: in a same queue situation
//											if (sor_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index)
//											{
//												//because we use barrier, so only one barrier is needed.
//												tar_pass->InsertSameQueueSyncInfo(
//													sor_pass,
//													tar_pass,
//													sync_event,
//													tar_pass_queue_cap,
//													sor_buf_usage->GetBufSynInfo(),
//													target_let.GetRsrcUsage()->GetBufSynInfo(),
//													target_let.GetUnderlyingRsrcItr());
//											}
//											//CASE 2: DIFF queue situation (need queue ownership transfer!)
//											else
//											{
//												sor_pass->InsertDiffQueueSyncInfoBinarySem(
//													sor_pass,
//													tar_pass,
//													sync_timeline_sema_same_level,
//													sor_pass_queue_ptr,
//													tar_pass_queue_ptr,
//													sor_buf_usage->GetBufSynInfo(),
//													target_let.GetRsrcUsage()->GetBufSynInfo(),
//													target_let.GetUnderlyingRsrcItr());
//
//												tar_pass->InsertDiffQueueSyncInfoBinarySem(
//													sor_pass,
//													tar_pass,
//													sync_timeline_sema_same_level,
//													sor_pass_queue_ptr,
//													tar_pass_queue_ptr,
//													sor_buf_usage->GetBufSynInfo(),
//													target_let.GetRsrcUsage()->GetBufSynInfo(),
//													target_let.GetUnderlyingRsrcItr());
//											}
//										}
//
//										else
//										{
//											throw std::runtime_error("Element not found in the unique_queues_set.");
//										}
//									}
//								}
//							}
//						}
//
//						//处理outlet buf
//						for (auto& [_, cur_out_let] : cur_pass->outs_buf)
//						{
//							const auto origin = cur_out_let.GetRsrcUsage()->GetRsrcOrigin();
//							if (RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu == origin)
//							{
//								auto source_sync = Vk::BufSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.offset = 0,
//									.size = VK_WHOLE_SIZE };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_out_let.GetRsrcUsage()->GetBufSynInfo(),
//									cur_out_let.GetUnderlyingRsrcItr());
//							}
//							else if (RsrcUsageInfoInPass::RsrcOrigin::FromOutSide == origin)
//							{
//								auto source_sync = Vk::BufSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.offset = 0,
//									.size = VK_WHOLE_SIZE };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_out_let.GetRsrcUsage()->GetBufSynInfo(),
//									cur_out_let.GetUnderlyingRsrcItr());
//							}
//
//							for (auto& [tar_pass, tar_let] : cur_out_let.target_outlet_itrs)
//							{
//								auto cur_pass_queue_ptr = pass_to_queue_info.at(cur_pass).first;
//								auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
//
//								auto cur_pass_queue_cap = cur_pass_queue_ptr->GetQueueCap();
//								auto tar_pass_queue_cap = tar_pass_queue_ptr->GetQueueCap();
//
//								auto& providing_outlet = cur_out_let;
//								auto& target_let = tar_let->second;
//
//								//Cur pass     ------------------------->       Tar pass
//								//Sync primitive
//								auto sync_event = same_queue_sync_event.at(PassPair{ cur_pass, tar_pass });
//								auto sync_sema = diff_queue_sync_sema.at(PassPair{ cur_pass, tar_pass });
//
//								//CASE 1: in a same queue situation
//								if (cur_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index)
//								{
//									//because we use barrier, so only one barrier is needed.
//									//cur_pass->InsertSameQueueSyncInfo(
//									//    cur_pass,
//									//    tar_pass,
//									//    sync_event,
//									//    cur_pass_queue_cap,
//									//    providing_outlet.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//								//CASE 2: DIFF queue situation (need queue ownership transfer!)
//								else
//								{
//									cur_pass->InsertDiffQueueSyncInfoBinarySem(
//										cur_pass,
//										tar_pass,
//										sync_sema,
//										cur_pass_queue_ptr,
//										tar_pass_queue_ptr,
//										providing_outlet.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//							}
//
//							for (auto& [tar_pass, tar_let] : cur_out_let.target_inlet_itrs)
//							{
//								auto cur_pass_queue_ptr = pass_to_queue_info.at(cur_pass).first;
//								auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
//
//								auto cur_pass_queue_cap = cur_pass_queue_ptr->GetQueueCap();
//								auto tar_pass_queue_cap = tar_pass_queue_ptr->GetQueueCap();
//
//								auto& providing_outlet = cur_out_let;
//								auto& target_let = tar_let->second;
//								//Cur pass     ------------------------->       Tar pass
//
//								//Sync primitive
//								auto sync_event = same_queue_sync_event.at(PassPair{ cur_pass, tar_pass });
//								auto sync_sema = diff_queue_sync_sema.at(PassPair{ cur_pass, tar_pass });
//
//								//CASE 1: in a same queue situation
//								if (cur_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index)
//								{
//									//because we use barrier, so only one barrier is needed.
//									//cur_pass->InsertSameQueueSyncInfo(
//									//    cur_pass,
//									//    tar_pass,
//									//    sync_event,
//									//    cur_pass_queue_cap,
//									//    providing_outlet.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//								//CASE 2: NOT in a same queue situation (need queue ownership transfer!)
//								else
//								{
//									cur_pass->InsertDiffQueueSyncInfoBinarySem(
//										cur_pass,
//										tar_pass,
//										sync_sema,
//										cur_pass_queue_ptr,
//										tar_pass_queue_ptr,
//										providing_outlet.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetRsrcUsage()->GetBufSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//							}
//						}
//
//						//处理无源的inlet，有源的会通过outlet处理
//						for (auto& [_, cur_in_let] : cur_pass->ins_uni_tex)
//						{
//							const auto origin = cur_in_let.GetRsrcUsage()->GetRsrcOrigin();
//							if (RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu == origin)
//							{
//								auto source_sync = Vk::TexSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED,
//									.img_subrange = std::nullopt };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_in_let.GetRsrcUsage()->GetTexSynInfo(),
//									cur_in_let.GetUnderlyingRsrcItr());
//							}
//							else if (RsrcUsageInfoInPass::RsrcOrigin::FromOutSide == origin)
//							{
//								auto source_sync = Vk::TexSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED,
//									.img_subrange = std::nullopt };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_in_let.GetRsrcUsage()->GetTexSynInfo(),
//									cur_in_let.GetUnderlyingRsrcItr());
//							}
//						}
//
//						//处理outlet
//						for (auto& [_, cur_out_let] : cur_pass->outs_uni_tex)
//						{
//							const auto origin = cur_out_let.GetRsrcUsage()->GetRsrcOrigin();
//							if (RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu == origin)
//							{
//								auto source_sync = Vk::TexSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED,
//									.img_subrange = std::nullopt };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_out_let.GetRsrcUsage()->GetTexSynInfo(),
//									cur_out_let.GetUnderlyingRsrcItr());
//							}
//							else if (RsrcUsageInfoInPass::RsrcOrigin::FromOutSide == origin)
//							{
//								auto source_sync = Vk::TexSyncInfo{
//									.access_mask = VK_ACCESS_NONE,
//									.stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//									.layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED,
//									.img_subrange = std::nullopt };
//
//								cur_pass->InsertInitalSyncInfo(
//									source_sync,
//									cur_out_let.GetRsrcUsage()->GetTexSynInfo(),
//									cur_out_let.GetUnderlyingRsrcItr());
//							}
//
//							for (auto& [tar_pass, tar_let] : cur_out_let.target_outlet_itrs)
//							{
//								auto cur_pass_queue_ptr = pass_to_queue_info.at(cur_pass).first;
//								auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
//
//								auto cur_pass_queue_cap = cur_pass_queue_ptr->GetQueueCap();
//								auto tar_pass_queue_cap = tar_pass_queue_ptr->GetQueueCap();
//
//								auto& providing_outlet = cur_out_let;
//								auto& target_let = tar_let->second;
//								//Cur pass     ------------------------->       Tar pass
//
//								//Sync primitive
//								auto sync_event = same_queue_sync_event.at(PassPair{ cur_pass, tar_pass });
//								auto sync_sema = diff_queue_sync_sema.at(PassPair{ cur_pass, tar_pass });
//
//								//CASE 1: in a same queue situation
//								if (cur_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index && cur_pass_queue_cap.queue_index == tar_pass_queue_cap.queue_index)
//								{
//									//because we use barrier, so only one barrier is needed.
//									//cur_pass->InsertSameQueueSyncInfo(
//									//    cur_pass,
//									//    tar_pass,
//									//    sync_event,
//									//    cur_pass_queue_cap,
//									//    providing_outlet.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//								//CASE 2: NOT in a same queue situation (need queue ownership transfer!)
//								else
//								{
//									cur_pass->InsertDiffQueueSyncInfoBinarySem(
//										cur_pass,
//										tar_pass,
//										sync_sema,
//										cur_pass_queue_ptr,
//										tar_pass_queue_ptr,
//										providing_outlet.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//							}
//
//							for (auto& [tar_pass, tar_let] : cur_out_let.target_inlet_itrs)
//							{
//								auto cur_pass_queue_ptr = pass_to_queue_info.at(cur_pass).first;
//								auto tar_pass_queue_ptr = pass_to_queue_info.at(tar_pass).first;
//
//								auto cur_pass_queue_cap = cur_pass_queue_ptr->GetQueueCap();
//								auto tar_pass_queue_cap = tar_pass_queue_ptr->GetQueueCap();
//
//								auto& providing_outlet = cur_out_let;
//								auto& target_let = tar_let->second;
//								//Cur pass     ------------------------->       Tar pass
//
//								//Sync primitive
//								auto sync_event = same_queue_sync_event.at(PassPair{ cur_pass, tar_pass });
//								auto sync_sema = diff_queue_sync_sema.at(PassPair{ cur_pass, tar_pass });
//
//								//CASE 1: in a same queue situation
//								if (cur_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index && cur_pass_queue_cap.queue_index == tar_pass_queue_cap.queue_index)
//								{
//									//because we use barrier, so only one barrier is needed.
//									//cur_pass->InsertSameQueueSyncInfo(
//									//    cur_pass,
//									//    tar_pass,
//									//    sync_event,
//									//    cur_pass_queue_cap,
//									//    providing_outlet.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetRsrcUsage()->GetSynInfo(),
//									//    target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//
//								//CASE 2: NOT in a same queue situation (need queue ownership transfer!)
//								else
//								{
//									cur_pass->InsertDiffQueueSyncInfoBinarySem(
//										cur_pass,
//										tar_pass,
//										sync_sema,
//										cur_pass_queue_ptr,
//										tar_pass_queue_ptr,
//										providing_outlet.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//
//									tar_pass->InsertSameQueueSyncInfo(
//										cur_pass,
//										tar_pass,
//										sync_event,
//										tar_pass_queue_cap,
//										providing_outlet.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetRsrcUsage()->GetTexSynInfo(),
//										target_let.GetUnderlyingRsrcItr());
//								}
//							}
//						}
//					}
//				}
//
//			);
//		}
//
//		//处理多个pass 同时 读 的同步问题
//	}
//
//	//void RenderGraphV0::DependencyGraph::SortAccessingPasses(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const
//	//{
//	//	std::sort(passes_access_this_rsrc.begin(), passes_access_this_rsrc.end(),
//	//	          [&final_exe_order](const PassNode *fir, const PassNode *sec) -> bool {
//	//		          const auto fir_itr = std::find(final_exe_order.begin(), final_exe_order.end(), fir);
//	//		          const auto sec_itr = std::find(final_exe_order.begin(), final_exe_order.end(), sec);
//	//		          return fir_itr < sec_itr;
//	//	          }
//	//
//	//	);
//	//}
//
//	VkRenderpassBaseRG::Type RenderGraphV0::GraphicsPassNode::GetRenderpassType()
//	{
//		return VkRenderpassBaseRG::Type::Graphics;
//	}
//
//	void RenderGraphV0::GraphicsPassNode::PreExecute()
//	{
//		renderpass->Init();
//	}
//
//	void RenderGraphV0::GraphicsPassNode::Execute(VkCommandBuffer cmd_buf)
//	{
//		renderpass->Execute(cmd_buf);
//	}
//
//}
