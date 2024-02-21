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
#include "Renderable.h"

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
				((pass0 == other.pass1) && (pass1 == other.pass0));
		}

		// Hash function
		struct Hash
		{
			size_t operator()(const PassNodePair& obj) const
			{
				// 使用哈希组合函数，确保指针的顺序不影响结果
				std::hash<GraphicsPassNode*> hasher;

				// 对两个指针进行哈希，使用异或操作
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

	enum PassType
	{
		None = 0x00,
		Graphics = 0x01,
		Compute = 0x02,
		Transfor = 0x04,
		Present = 0x04,
	};


	class GraphicsPassNode
	{
	private:

	public:
		GraphicsPassNode(
			std::string name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			VkShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,

			std::list<VirtualBuffer>& rg_buffers_,
			std::list<VirtualTexture>& rg_textures_

		);
		virtual ~GraphicsPassNode() = default;

	protected:
		std::string name;
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		DescriptorLayoutManager& descriptor_set_layout_manager;
		VkShaderFactory& shader_fac;
		DescriptorSetAllocatorGrowable& descriptor_allocator;

	protected:
		//虚拟资源查表
		std::unordered_map<std::string, VBufItr>  name_2_vbuf_itr;
		std::unordered_map<std::string, VTexItr>  name_2_vtex_itr;
		//虚拟资源存储
		std::list<VirtualBuffer>&  rg_buffers;
		std::list<VirtualTexture>& rg_textures;






	public:
		//资源初始化列表
		std::vector<VBufItr>    buf_init_list;
		std::vector<VTexItr>    tex_init_list;
		//销毁列表列表
		std::vector<VBufItr>    buf_recycle_list;
		std::vector<VTexItr>    tex_recycle_list;

		//资源使用方式 + 虚拟资源 打包在一起，更方便处理
		std::vector<VirtualBufRsrcAndUsage>            buf_usages;
		std::vector<VirtualTexRsrcAndUsage>            tex_usages;


	public:
		//pass的输入输出接口，其中包含着VirtualBufRsrcAndUsage的引用
		std::unordered_set<std::string> inlet_names;
		std::unordered_set<std::string> outlet_names;

		std::vector<RsrcOutlet<VirtualBufRsrcAndUsage>> outs_buf;
		std::vector<RsrcOutlet<VirtualTexRsrcAndUsage>> outs_tex;

		std::vector<RsrcInlet<VirtualBufRsrcAndUsage>> ins_buf;
		std::vector<RsrcInlet<VirtualTexRsrcAndUsage>> ins_tex;
	public://依赖相关
		std::unordered_set<GraphicsPassNode*> passes_depen_set;
		std::unordered_set<GraphicsPassNode*> passes_depen_on_cur_pass_set;
		size_t cur_level{ 0u };


		//同步
	protected:
		std::unordered_map<PassNodePair, std::pair<vk::Semaphore, SemInsertInfoSafe>, PassNodePair::Hash> pass_pair_to_t_sem;
		std::unordered_map<vk::Semaphore, SemInsertInfoSafe>                          timeline_sems;
		std::unordered_map<vk::Semaphore, SemInsertInfoSafe>                          binary_sems;

		//syncinfo暂时只用来作为command barrier insertion
		//same queue 同步primitive
	public:
		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualBuffer>>  buf_syn_infos_head_same_q;
		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_head_same_q;

		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualBuffer>>  buf_syn_infos_tail_same_q;
		std::vector<RenderGraphV1::SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_tail_same_q;


		//diff queue 同步primitive
	public:
		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualBuffer>>  buf_syn_infos_tail_diff_q;
		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualTexture>> tex_syn_infos_tail_diff_q;

		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualBuffer>>  buf_syn_infos_head_diff_q;
		std::vector<RenderGraphV1::SyncInfoDiffQueue<VirtualTexture>> tex_syn_infos_head_diff_q;


	public:
		//for model usage
		std::unordered_map<Anni::LoadedGLTF*, TexUsage> model_to_multi_tex_usage;

	public:
		void AddTimelineSyncSem(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info)
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


		void AddBinarySemSync(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info)
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





		void InsertSyncInfoForInitalLoad(const Anni::BufSyncInfo& source_syn_info, const Anni::BufSyncInfo& target_syn_info, VBufItr underlying_rsrc, std::shared_ptr<TimelineSemWrapper>& sem_on_load, SemInsertInfoSafe sem_insersion_info)
		{
			buf_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
			AddTimelineSyncSem(sem_on_load->GetRaw(), sem_insersion_info);
		}

		void InsertSyncInfoForInitalLoad(const Anni::ImgSyncInfo& source_syn_info, const Anni::ImgSyncInfo& target_syn_info, VTexItr underlying_rsrc, std::shared_ptr<TimelineSemWrapper>& sem_on_load, SemInsertInfoSafe sem_insersion_info)
		{
			tex_syn_infos_head_same_q.emplace_back(source_syn_info, target_syn_info, underlying_rsrc);
			AddTimelineSyncSem(sem_on_load->GetRaw(), sem_insersion_info);
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


		virtual void BeginRenderPass(vk::CommandBuffer cmd_buf)
		{

		}
		virtual void RecordCommandBuffer(const vk::CommandBuffer command_buffer)
		{

		}
		virtual void EndRenderPass(vk::CommandBuffer cmd_buf)
		{

		}




	public:
		void InsertSyncInfoForInitalUsage(
			const Anni::BufSyncInfo& source_syn_info,
			const Anni::BufSyncInfo& target_syn_info,
			VBufItr underlying_rsrc);

		void InsertSyncInfoForInitalUsage(
			const Anni::ImgSyncInfo& source_syn_info,
			const Anni::ImgSyncInfo& target_syn_info,
			VTexItr underlying_rsrc);


		void InsertSameQueueSyncInfo(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			Queue* const queue_,
			const Anni::BufSyncInfo& source_syn_info,
			const Anni::BufSyncInfo& target_syn_info,
			const VBufItr underlying_rsrc);

		void InsertSameQueueSyncInfo(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			Queue* const queue_,
			const Anni::ImgSyncInfo& source_syn_info,
			const Anni::ImgSyncInfo& target_syn_info,
			VTexItr underlying_rsrc);



		void InsertDiffQueueSyncInfoTimeline(
			GraphicsPassNode* const source_pass,
			GraphicsPassNode* const target_pass,
			std::shared_ptr<TimelineSemWrapper>& sema_sync,
			Queue* const source_queue_,
			Queue* const target_queue_,
			const Anni::BufSyncInfo& source_syn_info,
			const Anni::BufSyncInfo& target_syn_info,
			const VBufItr underlying_rsrc,

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
			VTexItr underlying_rsrc,

			SemInsertInfoSafe sor_sem_insersion_info,
			SemInsertInfoSafe tar_sem_insersion_info
		);


		void ResourcesAcquisition(
			VkTextureFactory& tex_fac,
			BufferFactory& buf_fac
		);


		void SyncInsertAhead(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);
		void SyncInsertAfter(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);

		void GenerateAllAccessStages();

	private:
		vk::PipelineStageFlags2 all_access_stages{ };

	private:
		void SyncInsertAheadSameQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);
		void SyncInsertAheadDiffQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);

	private:
		void SyncInsertAfterSameQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);
		void SyncInsertAfterDiffQ(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);

	public:
		virtual PassType GetRenderpassType();

	public:
		//************************************************************************************************
		VirtualTexture& GetVRsrcFromRsrcItr(const VTexItr tex_itr);
		VirtualBuffer&  GetVRsrcFromRsrcItr(const VBufItr buf_itr);
		//************************************************************************************************
		VBufItr CreateAndStoreVirtualBuffer(const std::string& underlying_vrsrc_name, std::shared_ptr<Buffer>& ptr_buf);
		VBufItr CreateAndStoreVirtualBuffer(const std::string& underlying_vrsrc_name, const Buffer::Descriptor& buf_descriptor);

		VTexItr CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name, std::shared_ptr<VkTexture>& ptr_tex);
		VTexItr CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name, const VkTexture::Descriptor& buf_descriptor);
		VTexItr CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name, std::vector<std::shared_ptr<VkTexture>>& model_textures);

		void AddCurPass2VRsrc(VBufItr vbuf_itr, RsrcAccessTypeRG access_t_);
		void AddCurPass2VRsrc(VTexItr vtex_itr, RsrcAccessTypeRG access_t_);

		//In和Out只代表资源是否会导出给其他pass使用，IN和OUT函数调用时使用的相关的资源 既可能会被读也可能会被写
		//In: 代表资源不会导出给其他pass使用，这通常不会引入dependency
		//Out:代表资源 会导出给其他pass使用

		//所有Buffer相关的输入输出函数
		//************************************************************************************************
		//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
		VirtualBufRsrcAndUsage& In(const std::string& rsrc_name, std::shared_ptr<Buffer> ptr_buf, BufUsage buf_usage);
		//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
		VirtualBufRsrcAndUsage& In(RsrcOutlet<VirtualBufRsrcAndUsage>& source_outlet, BufUsage buf_usage);
		//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
		RsrcOutlet<VirtualBufRsrcAndUsage>& Out(const std::string& rsrc_name, Buffer::Descriptor buf_descriptor, const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier, BufUsage buf_usage);
		//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
		RsrcOutlet<VirtualBufRsrcAndUsage>& Out(const std::string& rsrc_name, std::shared_ptr<Buffer>& ptr_buf, BufUsage buf_usage);
		//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
		RsrcOutlet<VirtualBufRsrcAndUsage>& Out(RsrcOutlet<VirtualBufRsrcAndUsage>& source_outlet, BufUsage buf_usage);

		//所有Texture相关的输入输出函数
		//************************************************************************************************
		//资源来自rendergraph之外，并且是来自模型的texture。
		VirtualTexRsrcAndUsage& In(const std::string& rsrc_name, std::vector<std::shared_ptr<VkTexture>>& model_textures, std::variant<TexUsage, AttachUsage> tex_usage);
		VirtualTexRsrcAndUsage& In(const std::string& rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage);
		VirtualTexRsrcAndUsage& In(RsrcOutlet<VirtualTexRsrcAndUsage>& source_outlet, std::variant<TexUsage, AttachUsage> tex_usage);
		RsrcOutlet<VirtualTexRsrcAndUsage>& Out(const std::string& rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier, std::variant<TexUsage, AttachUsage> tex_usage);

		RsrcOutlet<VirtualTexRsrcAndUsage>& Out(const std::string& rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::variant<TexUsage, AttachUsage> tex_usage);
		RsrcOutlet<VirtualTexRsrcAndUsage>& Out(RsrcOutlet<VirtualTexRsrcAndUsage>& source_outlet, std::variant<TexUsage, AttachUsage> tex_usage);
	};
}
