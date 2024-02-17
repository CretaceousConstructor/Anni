#pragma once
#include "Lets.h"
#include "VirtualRsrcAndUsage.h"
#include "VkShaderFactory.h"
#include "DesSetLayoutManager.h"
#include "DescriptorAllocator.h"
#include "RGSyncInfo.h"
#include "VkShaderFactory.h"
#include "TextureFactory.h"
#include "BufferFactory.h"
#include "Resource.h"

#include <functional>
namespace Anni::RenderGraphV1
{
	class GraphicsPassNode;
}

namespace Anni::RenderGraphV1
{
	class PassNodePair
	{
	public:
		PassNodePair(
			GraphicsPassNode* pass0_,
			GraphicsPassNode* pass1_) :
			pass0(pass0_), pass1(pass1_)
		{
		}

		// Equality operator
		bool operator==(const PassNodePair& other) const
		{
			return
				((pass0 == other.pass0) && (pass1 == other.pass1)) ||
				((pass0 == other.pass1) && (pass0 == other.pass0));
		}

		// Hash function
		struct Hash
		{
			size_t operator()(const PassNodePair& obj) const
			{
				// ʹ�ù�ϣ��Ϻ�����ȷ��ָ���˳��Ӱ����
				std::hash<GraphicsPassNode*> hasher;

				// ������ָ����й�ϣ��ʹ��������
				size_t hash1 = hasher(obj.pass0);
				size_t hash2 = hasher(obj.pass1);

				return hash1 ^ hash2;
			}
		};

	public:
		GraphicsPassNode* pass0;
		GraphicsPassNode* pass1;
	};
}


namespace Anni::RenderGraphV1
{
	class IPassNode
	{
	public:

	};

	enum PassType
	{
		None = 0x00,
		Graphics = 0x01,
		Compute = 0x02,
		Transfor = 0x04,
		Present = 0x04,
	};


	class GraphicsPassNode : public IPassNode
	{
	private:
		using VBufItr = std::unordered_map<std::string, VirtualBuffer>::iterator;
		using VTexItr = std::unordered_map<std::string, VirtualTexture>::iterator;

	public:
		GraphicsPassNode(
			std::string name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			VkShaderFactory& shader_fac_,
			DescriptorAllocatorGrowable& descriptor_allocator_,

			std::unordered_map<std::string, VirtualBuffer>& rg_buffers_map_,
			std::unordered_map<std::string, VirtualTexture>& rg_textures_map_
		);


	protected:
		std::string name;
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		DescriptorLayoutManager& descriptor_set_layout_manager;
		VkShaderFactory& shader_fac;
		DescriptorAllocatorGrowable& descriptor_allocator;

	protected:
		//������Դ
		std::unordered_map<std::string, VirtualBuffer>& rg_buffers_map;
		std::unordered_map<std::string, VirtualTexture>& rg_textures_map;



	public:
		//��Դ��ʼ���б�
		std::vector<VBufItr>    buf_init_list;
		std::vector<VTexItr>    tex_init_list;
		//�����б��б�
		std::vector<VBufItr>    buf_recycle_list;
		std::vector<VTexItr>    tex_recycle_list;

		//��Դʹ�÷�ʽ + ������Դ �����һ�𣬸����㴦��
		std::vector<VirtualBufRsrcAndUsage>            buf_usages;
		std::vector<VirtualTexRsrcAndUsage>            tex_usages;

		//pass����������ӿڣ����а�����VirtualBufRsrcAndUsage������
	public:
		std::unordered_map<std::string, RsrcOutlet<VirtualBufRsrcAndUsage>> outs_buf;
		std::unordered_map<std::string, RsrcOutlet<VirtualTexRsrcAndUsage>> outs_tex;

		std::unordered_map<std::string, RsrcInlet<VirtualBufRsrcAndUsage>> ins_buf;
		std::unordered_map<std::string, RsrcInlet<VirtualTexRsrcAndUsage>> ins_tex;
	public://�������
		std::unordered_set<GraphicsPassNode*> passes_depen_set;
		std::unordered_set<GraphicsPassNode*> passes_depen_on_cur_pass_set;
		size_t cur_level{ 0u };


		//ͬ��
	protected:
		std::unordered_map<PassNodePair, std::pair<vk::Semaphore, SemInsertInfoSafe>> pass_pair_to_t_sem;
		std::unordered_map<vk::Semaphore, SemInsertInfoSafe>                          t_sems;
		std::unordered_map<vk::Semaphore, SemInsertInfoSafe>                          b_sems;

		//syncinfo��ʱֻ������Ϊcommand barrier insertion
		//same queue ͬ��primitive
	public:
		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualBuffer>>  buf_syn_infos_tail_same_q;
		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_tail_same_q;

		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualBuffer>>  buf_syn_infos_head_same_q;
		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_head_same_q;

		//diff queue ͬ��primitive
	public:
		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualBuffer>>  buf_syn_infos_tail_diff_q;
		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualTexture>> tex_syn_infos_tail_diff_q;

		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualBuffer>>  buf_syn_infos_head_diff_q;
		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualTexture>> tex_syn_infos_head_diff_q;

	public:
		void AddTimelineSyncSem(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info)
		{
			if (!t_sems.contains(sem))
			{
				t_sems.emplace(sem, sem_insert_info);
			}
			else
			{
				VULKAN_HPP_ASSERT(t_sems[sem].wait_val == sem_insert_info.wait_val);
				VULKAN_HPP_ASSERT(t_sems[sem].signal_val == sem_insert_info.signal_val);
				t_sems[sem].wait_stages |= sem_insert_info.wait_stages;
				t_sems[sem].signal_stages |= sem_insert_info.signal_stages;
			}
		}


		void AddBinarySemSync(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info)
		{
			if (!b_sems.contains(sem))
			{
				b_sems.emplace(sem, sem_insert_info);
			}
			else
			{
				VULKAN_HPP_ASSERT(t_sems[sem].wait_val == sem_insert_info.wait_val);
				VULKAN_HPP_ASSERT(t_sems[sem].signal_val == sem_insert_info.signal_val);
				b_sems[sem].wait_stages |= sem_insert_info.wait_stages;
				b_sems[sem].signal_stages |= sem_insert_info.signal_stages;
			}
		}





		void InsertSyncInfoForInitalLoad(const Anni::BufSyncInfo& source_syn_info, const Anni::BufSyncInfo& target_syn_info, const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc, std::shared_ptr<TimelineSemWrapper>& sem_on_load, SemInsertInfoSafe sem_insersion_info)
		{
			buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
			AddTimelineSyncSem(sem_on_load->GetRaw(), sem_insersion_info);
			buf_syn_infos_head_same_q.back();
		}

		void InsertSyncInfoForInitalLoad(const Anni::ImgSyncInfo& source_syn_info, const Anni::ImgSyncInfo& target_syn_info, const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc, std::shared_ptr<TimelineSemWrapper>& sem_on_load, SemInsertInfoSafe sem_insersion_info)
		{
			tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc, sem_on_load);
			AddTimelineSyncSem(sem_on_load->GetRaw(), sem_insersion_info);
			tex_syn_infos_head_same_q.back();
		}


	public:

		virtual void CreateDescriptorSetLayout()
		{

		}
		virtual void AllocateDescriptorSets()
		{

		}
		virtual void UpdateDescriptorSets()
		{

		}
		virtual void GetShaders()
		{

		}


		virtual void BeginRenderPass(VkCommandBuffer cmd_buf)
		{

		}
		virtual void RecordCommandBuffer(const VkCommandBuffer command_buffer)
		{

		}
		virtual void EndRenderPass(VkCommandBuffer cmd_buf)
		{

		}




	public:
		void InsertSyncInfoForInitalUsage(
			const Anni::BufSyncInfo& source_syn_info,
			const Anni::BufSyncInfo& target_syn_info,
			const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc);

		void InsertSyncInfoForInitalUsage(
			const Anni::ImgSyncInfo& source_syn_info,
			const Anni::ImgSyncInfo& target_syn_info,
			const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc);


		void InsertSameQueueSyncInfo(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			Queue* const queue_,
			const Anni::BufSyncInfo& source_syn_info,
			const Anni::BufSyncInfo& target_syn_info,
			const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc);

		void InsertSameQueueSyncInfo(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			Queue* const queue_,
			const Anni::ImgSyncInfo& source_syn_info,
			const Anni::ImgSyncInfo& target_syn_info,
			const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc);



		void InsertDiffQueueSyncInfoTimeline(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			std::shared_ptr<TimelineSemWrapper>& sema_sync,
			Queue* const source_queue_,
			Queue* const target_queue_,
			const Anni::BufSyncInfo& source_syn_info,
			const Anni::BufSyncInfo& target_syn_info,
			const std::unordered_map<std::string, VirtualBuffer>::iterator underlying_rsrc,

			SemInsertInfoSafe sor_sem_insersion_info,
			SemInsertInfoSafe tar_sem_insersion_info
		);

		void InsertDiffQueueSyncInfoTimeline(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			std::shared_ptr<TimelineSemWrapper>& sema_sync,
			Queue* const source_queue_,
			Queue* const target_queue_,
			const Anni::ImgSyncInfo& source_syn_info,
			const Anni::ImgSyncInfo& target_syn_info,
			const std::unordered_map<std::string, VirtualTexture>::iterator underlying_rsrc,

			SemInsertInfoSafe sor_sem_insersion_info,
			SemInsertInfoSafe tar_sem_insersion_info
		);


		void ResourcesAcquisition(
			VkTextureFactory& tex_fac,
			BufferFactory& buf_fac
		);


		void SyncInsertAhead(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);
		void SyncInsertAfter(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);

		void GenerateAllAccessStages()
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

	private:
		vk::PipelineStageFlags2 all_access_stages{ };

	private:
		void SyncInsertAheadSameQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);
		void SyncInsertAheadDiffQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);

	private:
		void SyncInsertAfterSameQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
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
		void SyncInsertAfterDiffQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info)
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


	public:
		virtual PassType GetRenderpassType();
		//************************************************************************************************
		//In��Outֻ������Դ�Ƿ�ᵼ��������passʹ�ã�IN��OUT��������ʱʹ�õ���ص���Դ �ȿ��ܻᱻ��Ҳ���ܻᱻд
		//In: ������Դ���ᵼ��������passʹ�ã���ͨ����������dependency
		//Out:������Դ �ᵼ��������passʹ��

		//TODO����ν���������Ǩ?
		//����Buffer��ص������������
		//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��

		VirtualBufRsrcAndUsage& In(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, BufUsage buf_usage);

		//����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
		VirtualBufRsrcAndUsage& In(const std::string rsrc_name, GraphicsPassNode& source_pass, BufUsage buf_usage);

		//buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
		VirtualBufRsrcAndUsage& Out(const std::string rsrc_name, Buffer::Descriptor buf_descriptor, const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier, BufUsage buf_usage);

		//buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
		VirtualBufRsrcAndUsage& Out(const std::string rsrc_name, std::shared_ptr< Buffer> ptr_buf, BufUsage buf_usage);

		//����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
		VirtualBufRsrcAndUsage& Out(const std::string rsrc_name, GraphicsPassNode& source_pass, BufUsage buf_usage);

		//����Texture��ص������������
		//************************************************************************************************
		//��Դ����rendergraph֮�⣬����������ģ�͵�texture��
		VirtualTexRsrcAndUsage& In(const std::string rsrc_name, std::vector<std::shared_ptr<VkTexture>>& model_textures, std::variant<TexUsage, AttachUsage> tex_usage);

		VirtualTexRsrcAndUsage& In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage);

		VirtualTexRsrcAndUsage& In(const std::string rsrc_name, GraphicsPassNode& source_pass, std::variant<TexUsage, AttachUsage> tex_usage);

		VirtualTexRsrcAndUsage& Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier, std::variant<TexUsage, AttachUsage> tex_usage);

		VirtualTexRsrcAndUsage& Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage);

		VirtualTexRsrcAndUsage& Out(const std::string rsrc_name, GraphicsPassNode& source_pass, std::variant<TexUsage, AttachUsage> tex_usage);
	};
}
