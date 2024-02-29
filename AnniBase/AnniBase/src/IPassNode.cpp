#include "IPassNode.h"

namespace Anni::RenderGraphV1
{
	GraphicsPassNode::GraphicsPassNode(std::string name_, DeviceManager& device_manager_,
	                                   SwapchainManager& swapchain_manager_,
	                                   DescriptorLayoutManager& descriptor_set_layout_manager_,
	                                   VkShaderFactory& shader_fac_,
	                                   DescriptorSetAllocatorGrowable& descriptor_allocator_,
	                                   VkPipelineBuilder& pipeline_builder_,
	                                   std::vector<VirtualBuffer>& rg_buffers_,
	                                   std::vector<VirtualTexture>& rg_textures_
	) :
		name(std::move(name_)),
		device_manager(device_manager_),
		swapchain_manager(swapchain_manager_),

		descriptor_set_layout_manager(descriptor_set_layout_manager_),
		shader_fac(shader_fac_),
		descriptor_allocator(descriptor_allocator_),
		pipeline_builder(pipeline_builder_),
		rg_buffers(rg_buffers_),
		rg_textures(rg_textures_)
	{
	}


	void GraphicsPassNode::AddTimelineSyncSem(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info)
	{
		if (!timeline_sems.contains(sem))
		{
			timeline_sems.emplace(sem, sem_insert_info);
		}
		else
		{
			assert(timeline_sems.at(sem).wait_val == sem_insert_info.wait_val);
			assert(timeline_sems.at(sem).signal_val == sem_insert_info.signal_val);
			timeline_sems.at(sem).wait_stages |= sem_insert_info.wait_stages;
			timeline_sems.at(sem).signal_stages |= sem_insert_info.signal_stages;
		}
	}

	void GraphicsPassNode::AddBinarySemSync(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info)
	{
		if (!binary_sems.contains(sem))
		{
			binary_sems.emplace(sem, sem_insert_info);
		}
		else
		{
			assert(binary_sems.at(sem).wait_val == sem_insert_info.wait_val);
			assert(binary_sems.at(sem).signal_val == sem_insert_info.signal_val);
			binary_sems.at(sem).wait_stages |= sem_insert_info.wait_stages;
			binary_sems.at(sem).signal_stages |= sem_insert_info.signal_stages;
		}
	}

	void GraphicsPassNode::InsertSyncInfoForInitalLoad(const BufSyncInfo& source_syn_info,
	                                                   const BufSyncInfo& target_syn_info, VBufHandle underlying_rsrc,
	                                                   std::shared_ptr<TimelineSemWrapper>& sem_on_load,
	                                                   const SemInsertInfoSafe& sem_insersion_info)
	{
		buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
		AddTimelineSyncSem(sem_on_load->GetRaw(), sem_insersion_info);
	}

	void GraphicsPassNode::InsertSyncInfoForInitalLoad(const ImgSyncInfo& source_syn_info,
	                                                   const ImgSyncInfo& target_syn_info, VTexHandle underlying_rsrc,
	                                                   std::shared_ptr<TimelineSemWrapper>& sem_on_load,
	                                                   const SemInsertInfoSafe& sem_insersion_info)
	{
		tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
		AddTimelineSyncSem(sem_on_load->GetRaw(), sem_insersion_info);
	}

	void GraphicsPassNode::CreateDescriptorSetLayout()
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		for (auto& in_buf : ins_buf)
		{
			if (RsrcType::Buffer == in_buf.usage.GetRsrcType())
			{
				const DescriptorInfo desc_info = in_buf.usage.desc_info;
				vk::DescriptorSetLayoutBinding one_binding(
					desc_info.slot_info.binding, desc_info.descriptor_type,
					Vk::DescriptorCount<1>, desc_info.desc_shader_stages_flags);
				bindings.push_back(one_binding);
			}
		}


		for (auto& out_buf : ins_buf)
		{
			if (RsrcType::Buffer == out_buf.usage.GetRsrcType())
			{
				const DescriptorInfo desc_info = out_buf.usage.desc_info;
				vk::DescriptorSetLayoutBinding one_binding(
					desc_info.slot_info.binding, desc_info.descriptor_type,
					Vk::DescriptorCount<1>, desc_info.desc_shader_stages_flags);
				bindings.push_back(one_binding);
			}
		}


		for (auto& in_tex : ins_tex)
		{
			if (std::holds_alternative<TexUsage>(in_tex.usage))
			{
				TexUsage& texture_usage = std::get<TexUsage>(
					in_tex.usage); // Only works if v holds a TexUsage
				if (RsrcType::Texture == texture_usage.GetRsrcType())
				{
					const DescriptorInfo desc_info = texture_usage.desc_info;
					vk::DescriptorSetLayoutBinding one_binding(
						desc_info.slot_info.binding, desc_info.descriptor_type,
						Vk::DescriptorCount<1>, desc_info.desc_shader_stages_flags);

					bindings.push_back(one_binding);
				}
			}
		}

		for (auto& out_tex : ins_tex)
		{
			if (std::holds_alternative<TexUsage>(out_tex.usage))
			{
				TexUsage& texture_usage = std::get<TexUsage>(
					out_tex.usage); // Only works if v holds a TexUsage
				if (RsrcType::Texture == texture_usage.GetRsrcType())
				{
					const DescriptorInfo desc_info = texture_usage.desc_info;
					vk::DescriptorSetLayoutBinding one_binding(
						desc_info.slot_info.binding, desc_info.descriptor_type,
						Vk::DescriptorCount<1>, desc_info.desc_shader_stages_flags);

					bindings.push_back(one_binding);
				}
			}
		}

		vk::DescriptorSetLayoutCreateInfo desc_set_layout_CI{};
		desc_set_layout_CI.setBindings(bindings);

		common_layout =
			device_manager.GetLogicalDevice().createDescriptorSetLayoutUnique(
				desc_set_layout_CI);
	}

	void GraphicsPassNode::AllocateDescriptorSets()
	{
		common_descriptor_set = descriptor_allocator.Allocate(common_layout.get());
	}

	void GraphicsPassNode::UpdateDescriptorSets()
	{
		std::vector<vk::WriteDescriptorSet> all_writes;
		for (auto& in_buf : ins_buf)
		{
			if (RsrcType::Buffer == in_buf.usage.GetRsrcType())
			{
				all_writes.push_back(
					GetVRsrcFromRsrcHandle(in_buf.rsrc_handle).p_rsrc->GetWriteDescriptorSetInfo(
						in_buf.usage, common_descriptor_set));
			}
		}

		for (auto& in_buf : outs_buf)
		{
			if (RsrcType::Buffer == in_buf.usage.GetRsrcType())
			{
				all_writes.push_back(
					GetVRsrcFromRsrcHandle(in_buf.rsrc_handle).p_rsrc->GetWriteDescriptorSetInfo(
						in_buf.usage, common_descriptor_set));
			}
		}


		for (auto& in_tex : ins_tex)
		{
			if (std::holds_alternative<TexUsage>(in_tex.usage))
			{
				TexUsage& texture_usage = std::get<TexUsage>(in_tex.usage); // Only works if v holds a TexUsage
				if (RsrcType::Texture == texture_usage.GetRsrcType())
				{
					all_writes.push_back(
						GetVRsrcFromRsrcHandle(in_tex.rsrc_handle).p_rsrc->GetWriteDescriptorSetInfo(
							texture_usage, common_descriptor_set));
				}
			}
		}

		for (auto& out_tex : outs_tex)
		{
			if (std::holds_alternative<TexUsage>(out_tex.usage))
			{
				TexUsage& texture_usage = std::get<TexUsage>(out_tex.usage); // Only works if v holds a TexUsage
				if (RsrcType::Texture == texture_usage.GetRsrcType())
				{
					all_writes.push_back(
						GetVRsrcFromRsrcHandle(out_tex.rsrc_handle).p_rsrc->GetWriteDescriptorSetInfo(
							texture_usage, common_descriptor_set));
				}
			}
		}
		device_manager.GetLogicalDevice().updateDescriptorSets(all_writes, {});
	}

	void GraphicsPassNode::BeginRenderPass(vk::CommandBuffer cmd_buf)
	{
		vk::RenderingInfo rendering_info{};
		rendering_info.renderArea.offset = vk::Offset2D{0, 0};
		rendering_info.renderArea.extent =
			swapchain_manager.GetSwapChainImageExtent2D();
		rendering_info.layerCount = 1;

		std::vector<vk::RenderingAttachmentInfo> color_attachment_infos;
		vk::RenderingAttachmentInfo depth_attachment_info;
		vk::RenderingAttachmentInfo stensil_attachment_info;

		// 先把attachment的sampler 和 view 创建好。
		for (auto& in_tex : ins_tex)
		{
			if (std::holds_alternative<AttachUsage>(in_tex.usage))
			{
				ImgViewAndSamplerGeneration(in_tex);
			}
		}

		for (auto& out_tex : outs_tex)
		{
			if (std::holds_alternative<AttachUsage>(out_tex.usage))
			{
				ImgViewAndSamplerGeneration(out_tex);
			}
		}

		for (auto& in_tex : ins_tex)
		{
			if (std::holds_alternative<AttachUsage>(in_tex.usage))
			{
				BindResolveTarget(in_tex);
				AttachUsage& attach_usage = std::get<AttachUsage>(in_tex.usage); // Only works if v holds an AttachUsage
				const auto attach_type = attach_usage.attach_info.attach_type;
				if (AttachmentType::ColorAttachment == attach_type)
				{
					color_attachment_infos.push_back(
						attach_usage.GetVkRenderingAttachmentInfo());
				}
				if (AttachmentType::DepthAttachment == attach_type ||
					AttachmentType::DepthStencilAttachment == attach_type)
				{
					depth_attachment_info = attach_usage.GetVkRenderingAttachmentInfo();
				}
			}
		}

		for (auto& out_tex : outs_tex)
		{
			if (std::holds_alternative<AttachUsage>(out_tex.usage))
			{
				BindResolveTarget(out_tex);
				AttachUsage& attach_usage = std::get<AttachUsage>(out_tex.usage);
				// Only works if v holds an AttachUsage
				const auto attach_type = attach_usage.attach_info.attach_type;
				if (AttachmentType::ColorAttachment == attach_type)
				{
					color_attachment_infos.push_back(
						attach_usage.GetVkRenderingAttachmentInfo());
				}
				if (AttachmentType::DepthAttachment == attach_type ||
					AttachmentType::DepthStencilAttachment == attach_type)
				{
					depth_attachment_info = attach_usage.GetVkRenderingAttachmentInfo();
				}
			}
		}


		rendering_info.setColorAttachments(color_attachment_infos);
		rendering_info.setPDepthAttachment(&depth_attachment_info);

		cmd_buf.beginRendering(rendering_info);
	}

	void GraphicsPassNode::EndRenderPass(vk::CommandBuffer cmd_buf)
	{
		cmd_buf.endRendering();
	}

	void GraphicsPassNode::ImgViewAndSamplerGeneration(RsrcInlet<std::variant<TexUsage, AttachUsage>>& rsrc_usage)
	{
		const std::shared_ptr<VkTexture>& ptr_tex = GetVRsrcFromRsrcHandle(rsrc_usage.rsrc_handle).p_rsrc;
		std::variant<TexUsage, AttachUsage>& tex_usage = rsrc_usage.usage;

		std::visit(
			[&](auto& variant_usage)
			{
				if (!variant_usage.img_view_CI)
				{
					variant_usage.img_view = ptr_tex->GetTextureImageViewPtr();
				}
				else
				{
					variant_usage.img_view = std::make_shared<ImgViewWrapper>(
						device_manager, variant_usage.img_view_CI.value());
				}

				if (!variant_usage.sampler_CI)
				{
					variant_usage.sampler = ptr_tex->GetTextureSamplerPtr();
				}
				else
				{
					variant_usage.sampler = std::make_shared<SamplerWrapper>(
						device_manager, variant_usage.sampler_CI.value());
				}
			},
			tex_usage);
	}

	void GraphicsPassNode::ImgViewAndSamplerGeneration(RsrcOutlet<std::variant<TexUsage, AttachUsage>>& rsrc_usage)
	{
		const std::shared_ptr<VkTexture>& ptr_tex = GetVRsrcFromRsrcHandle(rsrc_usage.rsrc_handle).p_rsrc;
		std::variant<TexUsage, AttachUsage>& tex_usage = rsrc_usage.usage;

		std::visit(
			[&](auto& variant_usage)
			{
				if (!variant_usage.img_view_CI)
				{
					variant_usage.img_view = ptr_tex->GetTextureImageViewPtr();
				}
				else
				{
					variant_usage.img_view = std::make_shared<ImgViewWrapper>(
						device_manager, variant_usage.img_view_CI.value());
				}

				if (!variant_usage.sampler_CI)
				{
					variant_usage.sampler = ptr_tex->GetTextureSamplerPtr();
				}
				else
				{
					variant_usage.sampler = std::make_shared<SamplerWrapper>(
						device_manager, variant_usage.sampler_CI.value());
				}
			},
			tex_usage);
	}

	void GraphicsPassNode::BindResolveTarget(RsrcInlet<std::variant<TexUsage, AttachUsage>>& in_tex)
	{
		ASSERT_WITH_MSG(std::holds_alternative<std::monostate>(in_tex.resolve_target_let),
		                "No resolve target specified!");
		auto& cur_attach_usage = std::get<AttachUsage>(in_tex.usage);

		const std::variant<TexUsage, AttachUsage>* p_resovel_target = nullptr;

		if (std::holds_alternative<RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle>(in_tex.resolve_target_let))
		{
			const auto& resolve_target_let_handle = std::get<RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle>(
				in_tex.resolve_target_let);
			p_resovel_target = &resolve_target_let_handle.pass_attached_to->GetOulet(resolve_target_let_handle).usage;
		}

		if (std::holds_alternative<RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle>(in_tex.resolve_target_let))
		{
			const auto& resolve_target_let_handle = std::get<RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle>(
				in_tex.resolve_target_let);
			p_resovel_target = &resolve_target_let_handle.pass_attached_to->GetInlet(resolve_target_let_handle).usage;
		}

		const auto& resovel_target_usage = std::get<AttachUsage>(*p_resovel_target);
		cur_attach_usage.BindResolveTarget(resovel_target_usage.img_view, resovel_target_usage.sync_info.layout_inpass);
	}

	void GraphicsPassNode::BindResolveTarget(RsrcOutlet<std::variant<TexUsage, AttachUsage>>& out_tex)
	{
		ASSERT_WITH_MSG(std::holds_alternative<std::monostate>(out_tex.resolve_target_let),
		                "No resolve target specified!");
		auto& cur_attach_usage = std::get<AttachUsage>(out_tex.usage);

		const std::variant<TexUsage, AttachUsage>* p_resovel_target = nullptr;

		if (std::holds_alternative<RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle>(out_tex.resolve_target_let))
		{
			const auto& resolve_target_let_handle = std::get<RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle>(
				out_tex.resolve_target_let);
			p_resovel_target = &resolve_target_let_handle.pass_attached_to->GetOulet(resolve_target_let_handle).usage;
		}

		if (std::holds_alternative<RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle>(out_tex.resolve_target_let))
		{
			const auto& resolve_target_let_handle = std::get<RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle>(
				out_tex.resolve_target_let);
			p_resovel_target = &resolve_target_let_handle.pass_attached_to->GetInlet(resolve_target_let_handle).usage;
		}

		const auto& resovel_target_usage = std::get<AttachUsage>(*p_resovel_target);
		cur_attach_usage.BindResolveTarget(resovel_target_usage.img_view, resovel_target_usage.sync_info.layout_inpass);
	}

	void GraphicsPassNode::InsertSyncInfoForInitalUsage(
		const BufSyncInfo& source_syn_info,
		const BufSyncInfo& target_syn_info,
		VBufHandle underlying_rsrc)
	{
		buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}

	void GraphicsPassNode::InsertSyncInfoForInitalUsage(
		const ImgSyncInfo& source_syn_info,
		const ImgSyncInfo& target_syn_info,
		VTexHandle underlying_rsrc)
	{
		tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}


	void GraphicsPassNode::InsertSameQueueSyncInfo(
		const GraphicsPassNode* const source_pass,
		const GraphicsPassNode* const target_pass,
		Queue* const queue_,
		const BufSyncInfo& source_syn_info,
		const BufSyncInfo& target_syn_info,
		VBufHandle underlying_rsrc)
	{
		ASSERT_WITH_MSG(this == target_pass, "this sync info shouldn't be interted in this pass.");
		buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
	}

	void GraphicsPassNode::InsertSameQueueSyncInfo(
		const GraphicsPassNode* const source_pass,
		const GraphicsPassNode* const target_pass,
		Queue* const queue_,
		const ImgSyncInfo& source_syn_info,
		const ImgSyncInfo& target_syn_info,
		VTexHandle underlying_rsrc)
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
		const BufSyncInfo& source_syn_info,
		const BufSyncInfo& target_syn_info,
		VBufHandle underlying_rsrc,

		SemInsertInfoSafe sor_sem_insersion_info,
		SemInsertInfoSafe tar_sem_insersion_info)
	{
		if (this == source_pass)
		{
			buf_syn_infos_tail_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_,
			                                       underlying_rsrc);

			const auto key = PassNodePair{source_pass, target_pass};

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

			const auto key = PassNodePair{source_pass, target_pass};

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
		const ImgSyncInfo& source_syn_info,
		const ImgSyncInfo& target_syn_info,
		VTexHandle underlying_rsrc,

		SemInsertInfoSafe sor_sem_insersion_info,
		SemInsertInfoSafe tar_sem_insersion_info)
	{
		if (this == source_pass)
		{
			tex_syn_infos_tail_diff_q.emplace_back(source_syn_info, target_syn_info, source_queue_, target_queue_,
			                                       underlying_rsrc);

			const auto key = PassNodePair{source_pass, target_pass};

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

			const auto key = PassNodePair{source_pass, target_pass};

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
		for (const auto& buf_handle : buf_init_list)
		{
			//VirtualResource has a descriptor to help initalize resources
			buf_fac.ActualizeVirtualResource(GetVRsrcFromRsrcHandle(buf_handle));
		}

		for (const auto& tex_handle : tex_init_list)
		{
			//VirtualResource has a descriptor to help initalize resources
			tex_fac.ActualizeVirtualResource(GetVRsrcFromRsrcHandle(tex_handle));
		}
	}

	void GraphicsPassNode::SyncInsertAfter(vk::CommandBuffer cmd_buf,
	                                       std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
	{
		SyncInsertAfterSameQ(cmd_buf, signal_sem_submit_info);
		SyncInsertAfterDiffQ(cmd_buf, signal_sem_submit_info);
	}

	void GraphicsPassNode::GenerateAllAccessStages()
	{
		//std::ranges::for_each
		//(
		//	buf_usages,
		//	[&](const VBufHandleAndUsage& rsrc_usage)
		//	{
		//		all_access_stages |= rsrc_usage.usage.sync_info.stage_mask;
		//	}
		//);

		//std::ranges::for_each
		//(
		//	tex_usages,
		//	[&](const VTexHandleAndUsage& rsrc_usage)
		//	{
		//		all_access_stages |=
		//			std::visit
		//			([&](const auto& variant_usage) -> vk::PipelineStageFlags2
		//			 {
		//				 return variant_usage.sync_info.stage_mask;
		//			 }, rsrc_usage.usage
		//			);
		//	}
		//);
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
			VirtualBuffer& buf = GetVRsrcFromRsrcHandle(head_syn.underlying_vrsrc);
			vk::BufferMemoryBarrier2 buf_barrier = buf.GetBufBarrier(head_syn.source_sync_info,
			                                                         head_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		}


		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& head_syn : tex_syn_infos_head_same_q)
		{
			VirtualTexture& tex = GetVRsrcFromRsrcHandle(head_syn.underlying_vrsrc);
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
			VirtualBuffer& buf = GetVRsrcFromRsrcHandle(head_syn.underlying_vrsrc);
			vk::BufferMemoryBarrier2 buf_barrier = buf.GetBufBarrier(head_syn.source_sync_info,
			                                                         head_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		}


		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& head_syn : tex_syn_infos_head_diff_q)
		{
			VirtualTexture& tex = GetVRsrcFromRsrcHandle(head_syn.underlying_vrsrc);
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
			VirtualBuffer& buf = GetVRsrcFromRsrcHandle(tail_syn.underlying_vrsrc);

			vk::BufferMemoryBarrier2 buf_barrier = buf.GetBufBarrier(tail_syn.source_sync_info,
			                                                         tail_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		}

		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& tail_syn : tex_syn_infos_tail_same_q)
		{
			VirtualTexture& tex = GetVRsrcFromRsrcHandle(tail_syn.underlying_vrsrc);
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
			VirtualBuffer& buf = GetVRsrcFromRsrcHandle(tail_syn.underlying_vrsrc);
			vk::BufferMemoryBarrier2 buf_barrier = buf.GetBufBarrier(tail_syn.source_sync_info,
			                                                         tail_syn.target_sync_info);
			buf_bars.push_back(buf_barrier);
		}

		std::vector<vk::ImageMemoryBarrier2> tex_bars;
		for (auto& tail_syn : tex_syn_infos_tail_diff_q)
		{
			VirtualTexture& tex = GetVRsrcFromRsrcHandle(tail_syn.underlying_vrsrc);
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
		return None;
	}

	VirtualTexture& GraphicsPassNode::GetVRsrcFromRsrcHandle(const VTexHandle tex_handle)
	{
		return rg_textures[tex_handle.handle];
	}

	RsrcOutlet<BufUsage>& GraphicsPassNode::GetOulet(const RsrcOutlet<BufUsage>::Handle buf_outlet_handle)
	{
		return outs_buf[buf_outlet_handle.handle];
	}

	RsrcOutlet<std::variant<TexUsage, AttachUsage>>& GraphicsPassNode::GetOulet(
		const RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle tex_outlet_handle)
	{
		return outs_tex[tex_outlet_handle.handle];
	}

	RsrcInlet<BufUsage>& GraphicsPassNode::GetInlet(const RsrcInlet<BufUsage>::Handle buf_outlet_handle)
	{
		return ins_buf[buf_outlet_handle.handle];
	}

	RsrcInlet<std::variant<TexUsage, AttachUsage>>& GraphicsPassNode::GetInlet(
		const RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle tex_outlet_handle)
	{
		return ins_tex[tex_outlet_handle.handle];
	}

	VirtualBuffer& GraphicsPassNode::GetVRsrcFromRsrcHandle(const VBufHandle buf_handle)
	{
		return rg_buffers[buf_handle.handle];
	}


	VBufHandle GraphicsPassNode::CreateAndStoreVirtualBuffer(
		const std::string& underlying_vrsrc_name,
		std::shared_ptr<Buffer>& ptr_buf)
	{
		rg_buffers.emplace_back(underlying_vrsrc_name, ptr_buf);
		return VirtualBuffer::Handle(rg_buffers.size() - 1);
	}

	VBufHandle GraphicsPassNode::CreateAndStoreVirtualBuffer(
		const std::string& underlying_vrsrc_name,
		const Buffer::Descriptor& buf_descriptor)
	{
		rg_buffers.emplace_back(underlying_vrsrc_name, buf_descriptor);
		return VirtualBuffer::Handle(rg_buffers.size() - 1);
	}


	VTexHandle GraphicsPassNode::CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name,
	                                                          std::shared_ptr<VkTexture>& ptr_tex)
	{
		rg_textures.emplace_back(underlying_vrsrc_name, ptr_tex);
		return VTexHandle{rg_textures.size() - 1};
	}

	VTexHandle GraphicsPassNode::CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name,
	                                                          const VkTexture::Descriptor& buf_descriptor)
	{
		rg_textures.emplace_back(underlying_vrsrc_name, buf_descriptor);
		return VTexHandle{rg_textures.size() - 1};
	}

	VTexHandle GraphicsPassNode::CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name,
	                                                          std::vector<std::shared_ptr<VkTexture>>& model_textures)
	{
		rg_textures.emplace_back(underlying_vrsrc_name, &model_textures);
		return VTexHandle{rg_textures.size() - 1};
	}

	void GraphicsPassNode::AddCurPass2VRsrc(VBufHandle vbuf_handle, RsrcAccessTypeRG access_t_)
	{
		rg_buffers[vbuf_handle.handle].passes_access_this_rsrc.emplace_back(this, access_t_);
	}

	void GraphicsPassNode::AddCurPass2VRsrc(VTexHandle vtex_handle, RsrcAccessTypeRG access_t_)
	{
		rg_textures[vtex_handle.handle].passes_access_this_rsrc.emplace_back(this, access_t_);
	}


	RsrcInlet<BufUsage>::Handle GraphicsPassNode::In(
		const std::string& rsrc_name,
		std::shared_ptr<Buffer> ptr_buf,
		BufUsage buf_usage)
	{
		//决定资源的来源，之后还需要多一个类型用来处理诸如TAA的问题
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
		const auto underlying_rsrc_name = rsrc_name + "_";

		//首先看是否已经存在于buffers 容器中，如果没有就首次创建。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu（有没有提供descriptor，没有就不用实体化）
		auto [iterator, inserted] = name_2_vbuf_handle.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualBuffer(underlying_rsrc_name, ptr_buf);
		}
		const VBufHandle underlying_rsrc_handle = iterator->second;

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, buf_usage.access_type);

		//检查let重复
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In"; //把pass的名字作为后缀，创建导入导出口
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		//创建当前pass的资源的导入口
		ins_buf.emplace_back(buf_usage, underlying_rsrc_handle, this);

		//[此资源不需要在当前pass实体化]
		return RsrcInlet<BufUsage>::Handle{ins_buf.size(), this};
	}


	//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出.从其他pass的outlets中得到对应资源
	RsrcInlet<BufUsage>::Handle GraphicsPassNode::In(const RsrcOutlet<BufUsage>::Handle source_outlet_handle,
	                                                 BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
		RsrcOutlet<BufUsage>& source_outlet = source_outlet_handle.pass_attached_to->GetOulet(source_outlet_handle);

		const auto& rsrc_name = GetVRsrcFromRsrcHandle(source_outlet.GetUnderlyingRsrcHandle()).name;

		//创建当前pass的资源的导入口
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);
		auto& cur_inlet = ins_buf.emplace_back(buf_usage, source_outlet.GetUnderlyingRsrcHandle(), this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(source_outlet.GetUnderlyingRsrcHandle(), buf_usage.access_type);

		//给inlet赋值提供它的pass
		cur_inlet.AssignProvidingOutlet(source_outlet_handle);

		//[此资源不需要在当前pass实体化]
		return RsrcInlet<BufUsage>::Handle{ins_buf.size() - 1, this};
	}


	//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
	RsrcOutlet<BufUsage>::Handle GraphicsPassNode::Out(
		const std::string& rsrc_name,
		Buffer::Descriptor buf_descriptor,
		const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier,
		BufUsage buf_usage)
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
		assert(!name_2_vbuf_handle.contains(underlying_rsrc_name));
		const VBufHandle underlying_rsrc_handle = CreateAndStoreVirtualBuffer(underlying_rsrc_name, buf_descriptor);
		name_2_vbuf_handle.emplace(underlying_rsrc_name, underlying_rsrc_handle);

		//检查重复
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);


		//创建当前pass的资源的导出口
		outs_buf.emplace_back(buf_usage, underlying_rsrc_handle, this);


		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, buf_usage.access_type);

		//[此资源需要在当前pass实体化]
		buf_init_list.push_back(underlying_rsrc_handle);

		return RsrcOutlet<BufUsage>::Handle{outs_buf.size() - 1, this};
	}

	//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
	RsrcOutlet<BufUsage>::Handle GraphicsPassNode::Out(
		const std::string& rsrc_name,
		std::shared_ptr<Buffer>& ptr_buf,
		BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;

		const auto underlying_rsrc_name = rsrc_name + "_";
		//首先看imported virtual resource是否已经存在了
		auto [iterator, inserted] = name_2_vbuf_handle.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualBuffer(underlying_rsrc_name, ptr_buf);
		}
		const VBufHandle underlying_rsrc_handle = iterator->second;

		//检查重复
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);

		//创建当前pass的资源的导出口
		outs_buf.emplace_back(buf_usage, underlying_rsrc_handle, this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, buf_usage.access_type);

		//[此资源不需要在当前pass实体化]
		return RsrcOutlet<BufUsage>::Handle{outs_buf.size() - 1, this};
	}

	//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
	RsrcOutlet<BufUsage>::Handle GraphicsPassNode::Out(
		RsrcOutlet<BufUsage>::Handle source_outlet_handle,
		BufUsage buf_usage)
	{
		buf_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;

		RsrcOutlet<BufUsage>& source_outlet = source_outlet_handle.pass_attached_to->GetOulet(source_outlet_handle);
		const auto& rsrc_name = GetVRsrcFromRsrcHandle(source_outlet.GetUnderlyingRsrcHandle()).name;

		//创建当前pass的资源的导出口
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Inlet Exists!");
		outlet_names.insert(cur_outlet_name);

		auto& cur_outlet = outs_buf.emplace_back(buf_usage, source_outlet.GetUnderlyingRsrcHandle(), this);

		//给outlet赋值提供它的pass
		cur_outlet.AssignProvidingOutlet(source_outlet_handle);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(source_outlet.GetUnderlyingRsrcHandle(), buf_usage.access_type);

		//[此资源不需要在当前pass实体化]
		return RsrcOutlet<BufUsage>::Handle{outs_buf.size() - 1, this};
	}


	//所有Texture相关的输入输出函数
	//************************************************************************************************
	//资源来自rendergraph之外，并且是来自模型的texture。
	RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle GraphicsPassNode::In(
		const std::string& rsrc_name,
		std::vector<std::shared_ptr<VkTexture>>& model_textures,
		std::variant<TexUsage, AttachUsage> tex_usage)
	{
		const RsrcAccessTypeRG access_type =
			std::visit([&](auto& variant_usage) -> RsrcAccessTypeRG
			{
				variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
				return variant_usage.access_type;
			}, tex_usage);

		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph

		const auto underlying_rsrc_name = rsrc_name + "_";

		//首先看是否已经存在于textures 容器中，如果没有就重新创建虚拟资源。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu（有没有提供descriptor，没有就不用实体化）
		auto [iterator, inserted] = name_2_vtex_handle.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualTexture(underlying_rsrc_name, model_textures);
		}
		const VTexHandle underlying_rsrc_handle = iterator->second;

		//检查重复
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In"; //把pass的名字作为后缀，用来创建inlet和outlet
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		//创建当前pass的资源的导入口
		ins_tex.emplace_back(tex_usage, underlying_rsrc_handle, this);

		//给underlying resource增加使用它的pass，方便编译阶段处理
		AddCurPass2VRsrc(underlying_rsrc_handle, access_type);

		//[此资源不需要在当前pass实体化]
		return RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle{ins_tex.size() - 1, this};
	}


	RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle GraphicsPassNode::In(
		const std::string& rsrc_name,
		std::shared_ptr<VkTexture> ptr_tex,
		std::variant<TexUsage, AttachUsage> tex_usage)
	{
		RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
		{
			variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
			access_type = variant_usage.access_type;
		}, tex_usage);

		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
		const auto underlying_rsrc_name = rsrc_name + "_"; //无pass后缀

		//首先看是否已经存在于textures 容器中，如果没有就重新创建虚拟资源。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu（有没有提供descriptor，没有就不用实体化）
		auto [iterator, inserted] = name_2_vtex_handle.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualTexture(underlying_rsrc_name, ptr_tex);
		}
		const VTexHandle underlying_rsrc_handle = iterator->second;

		//检查重复
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In"; //把pass的名字作为后缀，用来创建inlet和outlet
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		//创建当前pass的资源的导入口
		ins_tex.emplace_back(tex_usage, underlying_rsrc_handle, this);

		//给underlying resource增加使用它的pass，方便编译阶段处理
		AddCurPass2VRsrc(underlying_rsrc_handle, access_type);

		//[此资源不需要在当前pass实体化]
		return RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle{ins_tex.size() - 1, this};
	}


	RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle GraphicsPassNode::In(
		RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle source_outlet_handle,
		std::variant<TexUsage, AttachUsage> tex_usage)
	{
		RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
		{
			variant_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
			access_type = variant_usage.access_type;
		}, tex_usage);

		RsrcOutlet<std::variant<TexUsage, AttachUsage>>& source_outlet = source_outlet_handle.pass_attached_to->
			GetOulet(source_outlet_handle);

		auto underlying_rsrc_handle = source_outlet.GetUnderlyingRsrcHandle();
		const auto& rsrc_name = GetVRsrcFromRsrcHandle(underlying_rsrc_handle).name;

		//创建当前pass的资源的导入口
		const auto cur_inlet_name = rsrc_name + "_" + this->name + "_In";
		ASSERT_WITH_MSG(!inlet_names.contains(cur_inlet_name), "Same Inlet Exists!");
		inlet_names.insert(cur_inlet_name);

		RsrcInlet<std::variant<TexUsage, AttachUsage>>& cur_inlet = ins_tex.emplace_back(
			tex_usage, underlying_rsrc_handle, this);

		//给ItrInRsrcMap增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, access_type);

		//给inlet赋值提供它的pass
		cur_inlet.AssignProvidingOutlet(source_outlet_handle);

		//[此资源不需要在当前pass实体化]
		return RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle{ins_tex.size() - 1, this};
	}

	RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle GraphicsPassNode::Out(
		const std::string& rsrc_name,
		VkTexture::Descriptor tex_descriptor,
		const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier,
		std::variant<TexUsage, AttachUsage> tex_usage)
	{
		RsrcAccessTypeRG access_type;
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
		assert(!name_2_vtex_handle.contains(underlying_rsrc_name));
		const VTexHandle underlying_rsrc_handle = CreateAndStoreVirtualTexture(underlying_rsrc_name, tex_descriptor);
		name_2_vtex_handle.emplace(underlying_rsrc_name, underlying_rsrc_handle);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, access_type);

		//创建当前pass的资源的导出口
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);

		outs_tex.emplace_back(tex_usage, underlying_rsrc_handle, this);

		//[此资源需要在当前pass实体化]
		tex_init_list.push_back(underlying_rsrc_handle);

		return RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle{outs_tex.size() - 1, this};
	}


	RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle GraphicsPassNode::Out(
		const std::string& rsrc_name,
		std::shared_ptr<VkTexture> ptr_tex,
		std::variant<TexUsage, AttachUsage> tex_usage)
	{
		RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
		{
			variant_usage.origin = IRsrcUsage::RsrcOrigin::FromOutSide;
			access_type = variant_usage.access_type;
		}, tex_usage);

		const auto underlying_rsrc_name = rsrc_name + "_";
		//首先看imported virtual resource是否已经存在了
		auto [iterator, inserted] = name_2_vtex_handle.try_emplace(underlying_rsrc_name);
		if (inserted)
		{
			// If the entry was inserted, it means it didn't exist
			iterator->second = CreateAndStoreVirtualTexture(underlying_rsrc_name, ptr_tex);
		}
		const VTexHandle underlying_rsrc_handle = iterator->second;

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, access_type);

		//创建当前pass的资源的导出口
		//检查重复
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Outlet Exists!");
		outlet_names.insert(cur_outlet_name);
		outs_tex.emplace_back(tex_usage, underlying_rsrc_handle, this);

		//[此资源不需要在当前pass实体化]
		return RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle{outs_tex.size() - 1, this};
	}


	RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle GraphicsPassNode::Out(
		RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle source_outlet_handle,
		std::variant<TexUsage, AttachUsage> tex_usage)
	{
		RsrcAccessTypeRG access_type;
		std::visit([&](auto& variant_usage)
		{
			variant_usage.origin = IRsrcUsage::RsrcOrigin::FromSourcePass;
			access_type = variant_usage.access_type;
		}, tex_usage);

		RsrcOutlet<std::variant<TexUsage, AttachUsage>>& source_outlet = source_outlet_handle.pass_attached_to->
			GetOulet(source_outlet_handle);
		const auto underlying_rsrc_handle = source_outlet.GetUnderlyingRsrcHandle();
		const auto& rsrc_name = GetVRsrcFromRsrcHandle(underlying_rsrc_handle).name;

		//创建当前pass的资源的导出口
		const auto cur_outlet_name = rsrc_name + "_" + this->name + "_Out";
		ASSERT_WITH_MSG(!outlet_names.contains(cur_outlet_name), "Same Inlet Exists!");
		outlet_names.insert(cur_outlet_name);

		auto& cur_outlet = outs_tex.emplace_back(tex_usage, underlying_rsrc_handle, this);

		//给underlying resource增加使用它的pass
		AddCurPass2VRsrc(underlying_rsrc_handle, access_type);

		//给outlet赋值提供它的pass
		cur_outlet.AssignProvidingOutlet(source_outlet_handle);

		//[此资源不需要在当前pass实体化]
		return RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle{outs_tex.size() - 1, this};
	}

	PassNodePair::PassNodePair(GraphicsPassNode* pass0_, GraphicsPassNode* pass1_):
		pass0(pass0_), pass1(pass1_)
	{
	}

	bool PassNodePair::operator==(const PassNodePair& other) const
	{
		return
			((pass0 == other.pass0) && (pass1 == other.pass1)) ||
			((pass0 == other.pass1) && (pass1 == other.pass0));
	}

	size_t PassNodePair::Hash::operator()(const PassNodePair& obj) const
	{
		// 使用哈希组合函数，确保指针的顺序不影响结果
		std::hash<GraphicsPassNode*> hasher;

		// 对两个指针进行哈希，使用异或操作
		size_t hash1 = hasher(obj.pass0);
		size_t hash2 = hasher(obj.pass1);

		return hash1 ^ hash2;
	}
}
