#pragma once
#include "Lets.h"
#include "ShaderFactory.h"
#include "DesSetLayoutManager.h"
#include "DescriptorAllocator.h"
#include "RGSyncInfo.h"
#include "TextureFactory.h"
#include "BufferFactory.h"
#include "Resource.h"
#include "Renderable.h"
#include "PipelineBuilder.h"

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
			GraphicsPassNode* pass1_);

		// Equality operator
		bool operator==(const PassNodePair& other) const;

		// Hash function
		struct Hash
		{
			size_t operator()(const PassNodePair& obj) const;
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
	public:
		GraphicsPassNode(
			std::string name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			ShaderFactory& shader_fac_,
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			PipelineBuilder& pipeline_builder_,
			std::vector<VirtualBuffer>& rg_buffers_,
			std::vector<VirtualTexture>& rg_textures_,
			std::unordered_map<std::string, VirtualBuffer::Handle>& rg_name_2_vbuf_handle_,
			std::unordered_map<std::string, VirtualTexture::Handle>& rg_name_2_vtex_handle_
		);
		virtual ~GraphicsPassNode() = default;

		GraphicsPassNode(const GraphicsPassNode&) = delete;
		GraphicsPassNode(GraphicsPassNode&&) = delete;
		GraphicsPassNode& operator=(const GraphicsPassNode&) = delete;
		GraphicsPassNode& operator=(GraphicsPassNode&&) = delete;

	protected:
		std::string name;
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		DescriptorLayoutManager& descriptor_set_layout_manager;
		ShaderFactory& shader_fac;
		DescriptorSetAllocatorGrowable& descriptor_allocator;
		PipelineBuilder& pipeline_builder;

	protected:
		//虚拟名字资源查表
		std::unordered_map<std::string, VirtualBuffer::Handle>& rg_name_2_vbuf_handle;
		std::unordered_map<std::string, VirtualTexture::Handle>& rg_name_2_vtex_handle;
		//虚拟资源存储
		std::vector<VirtualBuffer>& rg_buffers;
		std::vector<VirtualTexture>& rg_textures;

	public:
		////资源使用方式 + 虚拟资源 打包在一起，更方便处理
		//std::vector<VBufHandleAndUsage>            buf_usages;
		//std::vector<VTexHandleAndUsage>            tex_usages;

		//资源初始化列表
		std::vector<VirtualBuffer::Handle> buf_init_list;
		std::vector<VirtualTexture::Handle> tex_init_list;
		//销毁列表列表
		std::vector<VirtualBuffer::Handle> buf_recycle_list;
		std::vector<VirtualTexture::Handle> tex_recycle_list;

	public:
		//pass的输入输出接口，其中包含着VirtualBufRsrcAndUsage的引用
		std::unordered_set<std::string> inlet_names;
		std::unordered_set<std::string> outlet_names;

		std::vector<RsrcOutlet<BufUsage>> outs_buf;
		std::vector<RsrcOutlet<std::variant<TexUsage, AttachUsage>>> outs_tex;

		std::vector<RsrcInlet<BufUsage>> ins_buf;
		std::vector<RsrcInlet<std::variant<TexUsage, AttachUsage>>> ins_tex;

	public: //依赖相关
		std::unordered_set<GraphicsPassNode*> passes_depen_set;
		std::unordered_set<GraphicsPassNode*> passes_depen_on_cur_pass_set;
		size_t cur_level{0u};


		//同步
	protected:
		std::unordered_map<PassNodePair, std::pair<vk::Semaphore, SemInsertInfoSafe>, PassNodePair::Hash>
		pass_pair_to_t_sem;
		std::unordered_map<vk::Semaphore, SemInsertInfoSafe> timeline_sems;
		std::unordered_map<vk::Semaphore, SemInsertInfoSafe> binary_sems;

		//syncinfo暂时只用来作为command barrier insertion
		//same queue 同步primitive
	public:
		std::vector<SyncInfoSameQueue<VirtualBuffer>> buf_syn_infos_head_same_q;
		std::vector<SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_head_same_q;

		std::vector<SyncInfoSameQueue<VirtualBuffer>> buf_syn_infos_tail_same_q;
		std::vector<SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_tail_same_q;

		//diff queue 同步primitive
	public:
		std::vector<SyncInfoDiffQueue<VirtualBuffer>> buf_syn_infos_tail_diff_q;
		std::vector<SyncInfoDiffQueue<VirtualTexture>> tex_syn_infos_tail_diff_q;

		std::vector<SyncInfoDiffQueue<VirtualBuffer>> buf_syn_infos_head_diff_q;
		std::vector<SyncInfoDiffQueue<VirtualTexture>> tex_syn_infos_head_diff_q;

	public:
		std::vector<SyncInfoSameQueue<VirtualBuffer>> buf_syn_infos_initial_load;
		std::vector<SyncInfoSameQueue<VirtualTexture>> tex_syn_infos_initial_load;




	public:
		//for model usage
		std::unordered_map<LoadedGLTF*, TexUsage> model_to_multi_tex_usage;

	public:
		void AddTimelineSyncSem(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info);
		void AddBinarySemSync(vk::Semaphore sem, SemInsertInfoSafe sem_insert_info);


		void InsertSyncInfoForInitalLoad(const BufSyncInfo& source_syn_info, const BufSyncInfo& target_syn_info,
		                                 VBufHandle underlying_rsrc, std::shared_ptr<TimelineSemWrapper>& sem_on_load,
		                                 const SemInsertInfoSafe& sem_insersion_info);

		void InsertSyncInfoForInitalLoad(const ImgSyncInfo& source_syn_info, const ImgSyncInfo& target_syn_info,
		                                 VTexHandle underlying_rsrc, std::shared_ptr<TimelineSemWrapper>& sem_on_load,
		                                 const SemInsertInfoSafe& sem_insersion_info);

	public:

		//void GraphicsPassNode::InsertSyncInfoInitialUse(
		//	const BufSyncInfo& target_syn_info,
		//	VBufHandle underlying_rsrc,
		//	IRsrcUsage::RsrcOrigin rsrc_origin

		//);

		//void GraphicsPassNode::InsertSyncInfoInitialUse(
		//	const ImgSyncInfo& target_syn_info,
		//	VTexHandle underlying_rsrc,
		//	IRsrcUsage::RsrcOrigin rsrc_origin
		//);




		void InsertSyncInfoForInitalUsage(
			const BufSyncInfo& source_syn_info,
			const BufSyncInfo& target_syn_info,
			VBufHandle underlying_rsrc);

		void InsertSyncInfoForInitalUsage(
			const ImgSyncInfo& source_syn_info,
			const ImgSyncInfo& target_syn_info,
			VTexHandle underlying_rsrc);


		void InsertSameQueueSyncInfo(
			const GraphicsPassNode* source_pass,
			const GraphicsPassNode* target_pass,
			Queue* queue_,
			const BufSyncInfo& source_syn_info,
			const BufSyncInfo& target_syn_info,
			VBufHandle underlying_rsrc);

		void InsertSameQueueSyncInfo(
			const GraphicsPassNode* source_pass,
			const GraphicsPassNode* target_pass,
			Queue* queue_,
			const ImgSyncInfo& source_syn_info,
			const ImgSyncInfo& target_syn_info,
			VTexHandle underlying_rsrc);


		void InsertDiffQueueSyncInfoTimeline(
			GraphicsPassNode* source_pass,
			GraphicsPassNode* target_pass,
			std::shared_ptr<TimelineSemWrapper>& sema_sync,
			Queue* source_queue_,
			Queue* target_queue_,
			const BufSyncInfo& source_syn_info,
			const BufSyncInfo& target_syn_info,
			VBufHandle underlying_rsrc,

			SemInsertInfoSafe sor_sem_insersion_info,
			SemInsertInfoSafe tar_sem_insersion_info
		);

		void InsertDiffQueueSyncInfoTimeline(
			GraphicsPassNode* source_pass,
			GraphicsPassNode* target_pass,
			std::shared_ptr<TimelineSemWrapper>& sema_sync,
			Queue* source_queue_,
			Queue* target_queue_,
			const ImgSyncInfo& source_syn_info,
			const ImgSyncInfo& target_syn_info,
			VTexHandle underlying_rsrc,

			SemInsertInfoSafe sor_sem_insersion_info,
			SemInsertInfoSafe tar_sem_insersion_info
		);


		void ResourcesAcquisition(
			TextureFactory& tex_fac,
			BufferFactory& buf_fac
		);


		void SyncInsertAhead(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);
		void SyncInsertAfter(vk::CommandBuffer cmd_buf, std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);

		void GenerateAllAccessStages();

	private:
		vk::PipelineStageFlags2 all_access_stages{};

	private:
		void SyncInsertAheadSameQ(vk::CommandBuffer cmd_buf,
		                          std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);
		void SyncInsertAheadDiffQ(vk::CommandBuffer cmd_buf,
		                          std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_info);

	private:
		void SyncInsertAfterSameQ(vk::CommandBuffer cmd_buf,
		                          std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);
		void SyncInsertAfterDiffQ(vk::CommandBuffer cmd_buf,
		                          std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_info);

	public:
		virtual PassType GetRenderpassType() = 0;

	public:
		static vk::Format FindCommonFormatFromViewFormats(const std::vector<vk::Format>& view_formats);


		//************************************************************************************************
		VirtualBuffer& GetVRsrcFromRsrcHandle(VBufHandle buf_handle);
		VirtualTexture& GetVRsrcFromRsrcHandle(VTexHandle tex_handle);

		RsrcOutlet<BufUsage>& GetOulet(RsrcOutlet<BufUsage>::Handle buf_outlet_handle);
		RsrcOutlet<std::variant<TexUsage, AttachUsage>>& GetOulet(
			RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle tex_outlet_handle);

		RsrcInlet<BufUsage>& GetInlet(RsrcInlet<BufUsage>::Handle buf_outlet_handle);
		RsrcInlet<std::variant<TexUsage, AttachUsage>>& GetInlet(
			RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle tex_outlet_handle);
		//************************************************************************************************


		VBufHandle CreateAndStoreVirtualBuffer(const std::string& underlying_vrsrc_name,
		                                       std::shared_ptr<Buffer>& ptr_buf);
		VBufHandle CreateAndStoreVirtualBuffer(const std::string& underlying_vrsrc_name,
		                                       const Buffer::Descriptor& buf_descriptor);

		VTexHandle CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name,
		                                        std::shared_ptr<Texture>& ptr_tex);
		VTexHandle CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name,
		                                        const Texture::Descriptor& buf_descriptor);
		VTexHandle CreateAndStoreVirtualTexture(const std::string& underlying_vrsrc_name,
		                                        std::vector<std::shared_ptr<Texture>>& model_textures);

		void AddCurPass2VRsrc(VBufHandle vbuf_handle, RsrcAccessTypeRG access_t_);
		void AddCurPass2VRsrc(VTexHandle vtex_handle, RsrcAccessTypeRG access_t_);

		//In和Out只代表资源是否会导出给其他pass使用，IN和OUT函数调用时使用的相关的资源 既可能会被读也可能会被写
		//In: 代表资源不会导出给其他pass使用，这通常不会引入dependency
		//Out:代表资源 会导出给其他pass使用

		//所有Buffer相关的输入输出函数
		//************************************************************************************************
		//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
		RsrcInlet<BufUsage>::Handle In(const std::string& rsrc_name, std::shared_ptr<Buffer> ptr_buf,
		                               BufUsage buf_usage);
		//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
		RsrcInlet<BufUsage>::Handle In(RsrcOutlet<BufUsage>::Handle source_outlet_handle, BufUsage buf_usage);

		//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
		RsrcOutlet<BufUsage>::Handle Out(const std::string& rsrc_name, Buffer::Descriptor buf_descriptor,
		                                 const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier,
		                                 BufUsage buf_usage);
		//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
		RsrcOutlet<BufUsage>::Handle Out(const std::string& rsrc_name, std::shared_ptr<Buffer>& ptr_buf,
		                                 BufUsage buf_usage);
		//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
		RsrcOutlet<BufUsage>::Handle Out(RsrcOutlet<BufUsage>::Handle source_outlet_handle, BufUsage buf_usage);

		//所有Texture相关的输入输出函数
		//************************************************************************************************
		//资源来自rendergraph之外，并且是来自模型的texture。
		RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle In(const std::string& rsrc_name,
		                                                          std::vector<std::shared_ptr<Texture>>&
		                                                          model_textures,
		                                                          std::variant<TexUsage, AttachUsage> tex_usage);
		RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle In(const std::string& rsrc_name,
		                                                          std::shared_ptr<Texture> ptr_tex,
		                                                          std::variant<TexUsage, AttachUsage> tex_usage);
		RsrcInlet<std::variant<TexUsage, AttachUsage>>::Handle In(
			RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle source_outlet_handle,
			std::variant<TexUsage, AttachUsage> tex_usage);


		RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle Out(const std::string& rsrc_name,
		                                                            Texture::Descriptor tex_descriptor,
		                                                            const std::function<void(
			                                                            Texture::Descriptor& desco)>&
		                                                            descriptor_modifier,
		                                                            std::variant<TexUsage, AttachUsage> tex_usage);

		RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle Out(const std::string& rsrc_name,
		                                                            std::shared_ptr<Texture> ptr_tex,
		                                                            std::variant<TexUsage, AttachUsage> tex_usage);
		RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle Out(
			RsrcOutlet<std::variant<TexUsage, AttachUsage>>::Handle source_outlet_handle,
			std::variant<TexUsage, AttachUsage> tex_usage);

	protected:
		vk::UniqueDescriptorSetLayout common_layout;
		vk::DescriptorSet common_descriptor_set;

	public:
		virtual void CreateDescriptorSetLayout();
		virtual void AllocateDescriptorSets();
		virtual void UpdateDescriptorSets();
		virtual void GetShaders() = 0;
		virtual void BeginRenderPass(vk::CommandBuffer cmd_buf);
		virtual void RecordCommandBuffer(vk::CommandBuffer command_buffer) = 0;
		virtual void EndRenderPass(vk::CommandBuffer cmd_buf);

	protected:
		void ImgViewAndSamplerGeneration(RsrcInlet<std::variant<TexUsage, AttachUsage>>& rsrc_usage);
		void ImgViewAndSamplerGeneration(RsrcOutlet<std::variant<TexUsage, AttachUsage>>& rsrc_usage);

		static void BindResolveTarget(RsrcInlet<std::variant<TexUsage, AttachUsage>>& in_tex);
		static void BindResolveTarget(RsrcOutlet<std::variant<TexUsage, AttachUsage>>& out_tex);

	};
}
