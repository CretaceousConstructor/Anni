#pragma once
#include "SyncInfo.h"
#include "Vk.h"
#include "VkLets.h"
#include "VkRenderpassBaseRG.h"
#include "VkRsrcUsageInfoRG.h"

namespace Anni
{
	//namespace RenderGraphV0
	//{
	//	//template <typename R, typename S>
	//	//concept RsrcSyncInfoPair = (std::is_same_v<R, Buffer> && std::is_same_v<S, Vk::BufSyncInfo>) || (std::is_same_v<R, VkTexture> && std::is_same_v<S, Vk::TexSyncInfo>);

	//	class PassNode
	//	{
	//	public:
	//		PassNode(const std::string name_, GraphicsComponent& gfx_);

	//		virtual ~PassNode();

	//		PassNode(const PassNode&) = delete;
	//		PassNode& operator=(const PassNode&) = delete;

	//		PassNode(PassNode&&) = delete;
	//		PassNode& operator=(PassNode&&) = delete;

	//		void         InitRsrc();
	//		virtual void PreExecute() = 0;
	//		virtual void Execute(VkCommandBuffer cmd_buf) = 0;

	//		//Common to two methods(root pass marking and level dependency)
	//	public:
	//		virtual VkRenderpassBaseRG::Type GetRenderpassType();

	//		//Method: root pass marking
	//	public:
	//		void                     SetAsRootPass();
	//		void                     SetAsNonRootPass();
	//		bool                     IsRootPass();
	//		void                     TagCurRootPass(PassNode*);
	//		void                     AddSubGraphPass(PassNode*);
	//		void                     AccumulateQueueRequirement();
	//		VkRenderpassBaseRG::Type GetAccumulatedType();

	//	public:
	//		bool                           is_rootpass;
	//		PassNode* root_pass_tag;
	//		VkRenderpassBaseRG::Type       subgraph_accumulated_type;
	//		std::unordered_set<PassNode*> subgraph_pass;

	//		//Method: level dependency
	//	public:
	//		//template <typename R, typename S>
	//		//requires RsrcSyncInfoPair<R, S>
	//		//class SyncInfoSameQueue
	//		//{
	//		//  public:
	//		//	SyncInfoSameQueue(
	//		//	    VkEvent                                                       sync_event_,
	//		//	    S                                                             source_sync_info_,
	//		//	    S                                                             target_sync_info_,
	//		//	    std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_) :
	//		//	    sync_event(sync_event_),
	//		//	    source_sync_info(source_sync_info_),
	//		//	    target_sync_info(target_sync_info_),
	//		//	    underlying_rsrc(underlying_rsrc_)
	//		//	{
	//		//	}

	//		//	VkEvent                                                       sync_event;
	//		//	S                                                             source_sync_info;
	//		//	S                                                             target_sync_info;
	//		//	std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc;
	//		//};

	//		//template <typename R, typename S>
	//		//requires RsrcSyncInfoPair<R, S>
	//		//class SyncInfoDiffQueue
	//		//{
	//		//  public:
	//		//	SyncInfoDiffQueue(
	//		//	    VkSemaphore sync_sema_,
	//		//	    S           source_sync_info_,
	//		//	    S           target_sync_info_,

	//		//	    DeviceManager::VkExecutionQueue *source_queue_,
	//		//	    DeviceManager::VkExecutionQueue *target_queue_,

	//		//	    std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_) :
	//		//	    sync_sema(sync_sema_),
	//		//	    source_sync_info(source_sync_info_),
	//		//	    target_sync_info(target_sync_info_),

	//		//	    source_queue(source_queue_),
	//		//	    target_queue(target_queue_),

	//		//	    underlying_rsrc(underlying_rsrc_),
	//		//	    signal_value(std::nullopt),
	//		//	    wait_value(std::nullopt)
	//		//	{
	//		//	}

	//		//	VkSemaphore                                                   sync_sema;
	//		//	S                                                             source_sync_info;
	//		//	S                                                             target_sync_info;
	//		//	DeviceManager::VkExecutionQueue *                           source_queue;
	//		//	DeviceManager::VkExecutionQueue *                           target_queue;
	//		//	std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc;
	//		//	std::optional<uint64_t>                                       signal_value;
	//		//	std::optional<uint64_t>                                       wait_value;
	//		//};

	//		//Common variables
	//	public:
	//		std::unordered_set<PassNode*> passes_depen_set;
	//		std::unordered_set<PassNode*> passes_depen_on_cur_pass_set;

	//	public:
	//		std::string name{};

	//	public:
	//		//资源使用方式
	//		std::vector<VkRsrcUsageInfoRGBuf>          buf_usages;
	//		std::vector<VkRsrcUsageInfoRGUniversalTex> uni_tex_usages;

	//	public:
	//		//资源初始化列表
	//		std::vector<std::unordered_map<std::string, VirtualResource<Buffer>>::iterator>    buf_init_list;
	//		std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> uni_tex_init_list;

	//	public:
	//		//销毁列表列表
	//		std::vector<std::unordered_map<std::string, VirtualResource<Buffer>>::iterator>    buf_recycle_list;
	//		std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> uni_tex_recycle_list;

	//		//输入输出接口
	//	public:
	//		std::unordered_map<std::string, RsrcOutlet<Buffer, VkBufUsageInfoRG>>             outs_buf;
	//		std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>> outs_uni_tex;

	//		std::unordered_map<std::string, RsrcInlet<Buffer, VkBufUsageInfoRG>>             ins_buf;
	//		std::unordered_map<std::string, RsrcInlet<VkTexture, VkUniversalTexUsageInfoRG>> ins_uni_tex;

	//		//level dependency
	//		size_t cur_level;

	//		//same queue 同步primitive
	//	public:
	//		//cur_pass -> set_event -> wait_event -> target_pass
	//		std::vector<SyncInfoSameQueue<Buffer, Vk::BufSyncInfo>>    buf_syn_infos_tail_same_q;
	//		std::vector<SyncInfoSameQueue<VkTexture, Vk::TexSyncInfo>> uni_tex_syn_infos_tail_same_q;

	//		//source_pass -> set_event -> wait_event -> cur_pass
	//		std::vector<SyncInfoSameQueue<Buffer, Vk::BufSyncInfo>>    buf_syn_infos_head_same_q;
	//		std::vector<SyncInfoSameQueue<VkTexture, Vk::TexSyncInfo>> uni_tex_syn_infos_head_same_q;

	//		//diff queue 同步primitive
	//	public:
	//		//cur_pass -> set_event -> wait_event -> target_pass
	//		std::vector<SyncInfoDiffQueue<Buffer, Vk::BufSyncInfo>>    buf_syn_infos_tail_diff_q;
	//		std::vector<SyncInfoDiffQueue<VkTexture, Vk::TexSyncInfo>> uni_tex_syn_infos_tail_diff_q;

	//		//source_pass -> set_event -> wait_event -> cur_pass
	//		std::vector<SyncInfoDiffQueue<Buffer, Vk::BufSyncInfo>>    buf_syn_infos_head_diff_q;
	//		std::vector<SyncInfoDiffQueue<VkTexture, Vk::TexSyncInfo>> uni_tex_syn_infos_head_diff_q;

	//		void SyncInsertAhead(VkCommandBuffer cmd_buf);
	//		void SyncInsertAfter(VkCommandBuffer cmd_buf);

	//		void InsertInitalSyncInfo(
	//			Vk::BufSyncInfo                                                    source_syn_info,
	//			Vk::BufSyncInfo                                                    target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc);

	//		void InsertInitalSyncInfo(
	//			Vk::TexSyncInfo                                                       source_syn_info,
	//			Vk::TexSyncInfo                                                       target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc);

	//		void InsertSameQueueSyncInfo(
	//			PassNode* source_pass,
	//			PassNode* target_pass,
	//			VkEvent                                                            event_sync,
	//			Queue* queue_,
	//			Vk::BufSyncInfo                                                    source_syn_info,
	//			Vk::BufSyncInfo                                                    target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc);

	//		void InsertSameQueueSyncInfo(
	//			PassNode* source_pass,
	//			PassNode* target_pass,
	//			VkEvent                                                               event_sync,
	//			Queue* queue_,
	//			Vk::TexSyncInfo                                                       source_syn_info,
	//			Vk::TexSyncInfo                                                       target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc);

	//		//Watch out for queue ownership transfer!
	//		void InsertDiffQueueSyncInfoBinarySem(
	//			PassNode* source_pass,
	//			PassNode* target_pass,
	//			VkSemaphore                                                           sema_sync,
	//			Queue* source_queue_,
	//			Queue* target_queue_,
	//			Vk::TexSyncInfo                                                       source_syn_info,
	//			Vk::TexSyncInfo                                                       target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc

	//		);

	//		void InsertDiffQueueSyncInfoBinarySem(
	//			PassNode* source_pass,
	//			PassNode* target_pass,
	//			VkSemaphore                                                        sema_sync,
	//			Queue* source_queue_,
	//			Queue* target_queue_,
	//			Vk::BufSyncInfo                                                    source_syn_info,
	//			Vk::BufSyncInfo                                                    target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc

	//		);

	//		void InsertDiffQueueSyncInfoTimeline(
	//			PassNode* source_pass,
	//			PassNode* target_pass,
	//			VkSemaphore                                                        sema_sync,
	//			uint64_t                                                           wait_value,
	//			uint64_t                                                           signal_value,
	//			Queue* source_queue_,
	//			Queue* target_queue_,
	//			Vk::BufSyncInfo                                                    source_syn_info,
	//			Vk::BufSyncInfo                                                    target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<Buffer>>::iterator underlying_rsrc

	//		);

	//		void InsertDiffQueueSyncInfoTimeline(
	//			PassNode* source_pass,
	//			PassNode* target_pass,
	//			VkSemaphore                                                           sema_sync,
	//			uint64_t                                                              wait_value,
	//			uint64_t                                                              signal_value,
	//			Queue* source_queue_,
	//			Queue* target_queue_,
	//			Vk::BufSyncInfo                                                       source_syn_info,
	//			Vk::BufSyncInfo                                                       target_syn_info,
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator underlying_rsrc

	//		);

	//	private:
	//		GraphicsComponent& gfx;
	//		VkRenderpassManager& renderpass_manager;
	//	};

	//	class GraphicsPassNode : public PassNode
	//	{
	//	public:
	//		GraphicsPassNode(const std::string                                            name_,
	//			std::shared_ptr<VkRenderpassBaseRG>                          renderpass_,
	//			std::unordered_map<std::string, VirtualResource<Buffer>>& buffers_map,
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>& uni_textures_map) :
	//			PassNode(name_),
	//			rg_buffers_map(buffers_map),
	//			rg_uni_textures_map(uni_textures_map),
	//			renderpass(renderpass_)
	//		{
	//		}

	//	private:
	//		std::unordered_map<std::string, VirtualResource<Buffer>>& rg_buffers_map;
	//		std::unordered_map<std::string, VirtualResource<VkTexture>>& rg_uni_textures_map;

	//	private:
	//		std::shared_ptr<VkRenderpassBaseRG> renderpass;

	//	public:
	//		~GraphicsPassNode() override = default;

	//		GraphicsPassNode(const GraphicsPassNode&) = delete;
	//		GraphicsPassNode& operator=(const GraphicsPassNode&) = delete;
	//		GraphicsPassNode(GraphicsPassNode&&) = delete;
	//		GraphicsPassNode& operator=(GraphicsPassNode&&) = delete;

	//	public:
	//		VkRenderpassBaseRG::Type GetRenderpassType() override;

	//		//************************************************************************************************
	//		//In和Out只代表资源是否会导出给其他pass使用，IN和OUT函数调用时使用的相关的资源 既可能会被读也可能会被写

	//		//In: 代表资源不会导出给其他pass使用，这通常不会引入dependency
	//		//Out:代表资源 会导出给其他pass使用

	//		//TODO：如何进行命名变迁?
	//		//所有Buffer相关的输入输出函数
	//		//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出

	//		GraphicsPassNode& In(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			//决定资源的来源，之后还需要多一个类型用来处理诸如TAA的问题
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);

	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//首先看是否已经存在于buffers map中，如果没有就首次创建。注意这里创建的是虚拟资源，至于需不需要实体化，要看是不是EstablishedInSitu

	//			auto rsrc_itr = rg_buffers_map.find(underlying_rsrc_name);
	//			if (rsrc_itr == rg_buffers_map.end())
	//			{
	//				const auto temp_itr = rg_buffers_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_buf);        //键值和emplace构造元素所需的参数
	//				rsrc_itr = temp_itr.first;
	//			}

	//			const auto inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀，创建导入导出口

	//			//创建当前pass的资源的导入口
	//			const auto cur_inlet_itr = ins_buf.try_emplace(inlet_name, rsrc_itr, this, buf_usage.get());
	//			if (!cur_inlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Inlet!");
	//			}

	//			//给underlying resource增加使用它的pass
	//			rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//资源使用方式存储。move unique ptr并不会改变其底部对象的地址
	//			this->buf_usages.emplace_back(std::move(buf_usage), rsrc_itr);

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
	//		GraphicsPassNode& In(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);

	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //如GbufferPos_GPass_Out

	//			//从其他pass的outlets中寻找对应资源
	//			const auto source_outlet_itr = source_pass.outs_buf.find(source_outlet_name);
	//			if (source_pass.outs_buf.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源，建议直接crash
	//			{
	//				throw std::runtime_error("resource with the name doesn't exist!");
	//			}

	//			//给ItrInRsrcMap 增加使用它的pass
	//			source_outlet_itr->second.GetUnderlyingRsrcItr()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导入口
	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");
	//			const auto cur_inlet_itr = ins_buf.try_emplace(cur_inlet_name, source_outlet_itr->second.GetUnderlyingRsrcItr(), this, buf_usage.get());

	//			if (!cur_inlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Inlet!");
	//			}

	//			//给source outlet增加使用它的pass，以及使用它的pass对应的使用方式
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_inlet_itr.first);

	//			//给inlet赋值提供它的pass
	//			cur_inlet_itr.first->second.AssignProvidingPassAndProvidingOutItr(&source_pass, source_outlet_itr);

	//			//资源使用方式存储
	//			this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetUnderlyingRsrcItr());

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
	//		GraphicsPassNode& Out(const std::string rsrc_name, Buffer::Descriptor buf_descriptor, const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu);
	//			//当前资源含有descriptor，资源就是在当前pass创建。

	//			//资源的命名方式为：资源名 + _
	//			const auto rsrc_name = rsrc_name + std::string("_");

	//			//用传入的lambda函数改变descriptor
	//			descriptor_modifier(buf_descriptor);

	//			//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
	//			const auto underlying_rsrc_itr = rg_buffers_map.try_emplace(rsrc_name, rsrc_name, buf_descriptor);
	//			//确保用户没有重复添加
	//			if (!underlying_rsrc_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//给underlying resource增加使用它的pass
	//			underlying_rsrc_itr.first->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导出口
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_out_itr = outs_buf.try_emplace(cur_outlet_name, underlying_rsrc_itr.first, this, buf_usage.get());

	//			if (!cur_out_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exists in Outlet!");
	//			}

	//			//资源使用方式存储
	//			this->buf_usages.emplace_back(std::move(buf_usage), underlying_rsrc_itr.first);

	//			//[此资源需要在当前pass实体化]
	//			buf_init_list.push_back(underlying_rsrc_itr.first);

	//			return *this;
	//		}

	//		//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
	//		GraphicsPassNode& Out(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);

	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//首先看imported virtual resource是否已经存在了
	//			auto underlying_rsrc_itr = rg_buffers_map.find(underlying_rsrc_name);
	//			//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果

	//			//如果virtual resource不存在，则创建:
	//			if (underlying_rsrc_itr == rg_buffers_map.end())
	//			{
	//				const auto temp_itr = rg_buffers_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_buf);        //键值和emplace构造所需的参数
	//				underlying_rsrc_itr = temp_itr.first;
	//			}

	//			//给underlying resource增加使用它的pass
	//			underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导出口
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto outlet_itr = outs_buf.try_emplace(cur_outlet_name, underlying_rsrc_itr, this, buf_usage.get());

	//			if (!outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Outlet!");
	//			}

	//			//资源使用方式存储
	//			this->buf_usages.emplace_back(std::move(buf_usage), underlying_rsrc_itr);

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
	//		GraphicsPassNode& Out(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);

	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

	//			//先找一找  源头pass的导出接口是否确实提供了需要的资源

	//			const auto source_outlet_itr = source_pass.outs_buf.find(source_outlet_name);
	//			if (source_pass.outs_buf.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
	//			{
	//				throw std::runtime_error("resource with the name doesn't exist in source outlet!");        //检查不通过，直接报错，抛出异常
	//			}

	//			//给ItrInRsrcMap增加使用它的pass
	//			source_outlet_itr->second.GetUnderlyingRsrcItr()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导出口
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_outlet_itr = outs_buf.try_emplace(cur_outlet_name, source_outlet_itr->second.GetUnderlyingRsrcItr(), this, buf_usage.get());

	//			if (!cur_outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Outlet!");
	//			}

	//			//source pass中的outlet添加accessing pass
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_outlet_itr.first);

	//			//把资源使用方式存储起来
	//			this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetUnderlyingRsrcItr());

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		//************************************************************************************************

	//		//所有Texture相关的输入输出函数
	//		//************************************************************************************************
	//		//资源来自rendergraph之外，并且是来自模型的texture。
	//		GraphicsPassNode& In(const std::string rsrc_name, std::vector<std::shared_ptr<VkTexture>> model_textures, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);
	//			//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph

	//			std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> model_tex_rsrc_itrs;
	//			model_tex_rsrc_itrs.reserve(model_textures.size());

	//			for (auto& ptr_tex : model_textures)
	//			{
	//				const auto underlying_rsrc_name = rsrc_name + std::string("_") + ptr_tex->GetTextureName();        //无pass后缀
	//				//首先看是否已经存在于textures map中，如果没有就重新创建
	//				auto underlying_rsrc_itr = rg_uni_textures_map.find(underlying_rsrc_name);

	//				if (underlying_rsrc_itr == rg_uni_textures_map.end())
	//				{
	//					const auto temp_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //键值和emplace构造元素所需的参数
	//					underlying_rsrc_itr = temp_itr.first;
	//				}

	//				model_tex_rsrc_itrs.push_back(underlying_rsrc_itr);
	//			}

	//			//给ItrInRsrcMap增加使用它的pass，方便编译阶段处理
	//			for (const auto& underlying_rsrc_itr : model_tex_rsrc_itrs)
	//			{
	//				underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());
	//			}

	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀，用来创建inlet和outlet
	//			//创建当前pass的资源的导入口
	//			const auto cur_inlet_in_itr = ins_uni_tex.try_emplace(cur_inlet_name, model_tex_rsrc_itrs, this, uni_tex_usage.get());
	//			if (!cur_inlet_in_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//资源使用方式存储
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), std::move(model_tex_rsrc_itrs));

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		GraphicsPassNode& In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);
	//			//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");        //无pass后缀
	//			//首先看是否已经存在于textures map中，如果没有就重新创建
	//			auto underlying_rsrc_itr = rg_uni_textures_map.find(underlying_rsrc_name);
	//			if (underlying_rsrc_itr == rg_uni_textures_map.end())
	//			{
	//				const auto temp_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //键值和emplace构造元素所需的参数
	//				underlying_rsrc_itr = temp_itr.first;
	//			}

	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀，用来创建inlet和outlet

	//			//创建当前pass的资源的导入口
	//			const auto cur_inlet_in_itr = ins_uni_tex.try_emplace(cur_inlet_name, underlying_rsrc_itr, this, uni_tex_usage.get());
	//			if (!cur_inlet_in_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//给ItrInRsrcMap增加使用它的pass，方便编译阶段处理
	//			underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//资源使用方式存储
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), underlying_rsrc_itr);

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		GraphicsPassNode& In(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);

	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //如GbufferPos_GPass_Out

	//			//从其他pass的outlets中寻找对应资源[texture可以导出后被继续用作attach，attach也可以导出后被继续用作attach]
	//			const auto source_outlet_itr = source_pass.outs_uni_tex.find(source_outlet_name);        //如GbufferPos_GPass_Out

	//			if (source_pass.outs_uni_tex.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
	//			{
	//				const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  isn't provided by the source pass outlet!");
	//				throw std::runtime_error(err_meg.c_str());
	//			}

	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			vrsrc_itr = source_outlet_itr->second.GetUnderlyingRsrcItr();

	//			//给ItrInRsrcMap增加使用它的pass
	//			vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导入口
	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");
	//			const auto cur_inlet_itr = ins_uni_tex.try_emplace(cur_inlet_name, vrsrc_itr, this, uni_tex_usage.get());

	//			if (!cur_inlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//给source outlet增加使用它的pass
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_inlet_itr.first);

	//			//给inlet赋值提供它的pass
	//			cur_inlet_itr.first->second.AssignProvidingPassAndProvidingOutItr(&source_pass, source_outlet_itr);

	//			//资源使用方式存储
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), vrsrc_itr);

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		GraphicsPassNode& Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu);
	//			//当前函数传入参数含有descriptor，资源就是在当前pass创建。

	//			//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//用传入的lambda函数改变descriptor
	//			descriptor_modifier(tex_descriptor);

	//			//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
	//			const auto underlying_rsrc_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, tex_descriptor);
	//			//确保用户没有重复添加
	//			if (!underlying_rsrc_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//给underlying resource增加使用它的pass
	//			underlying_rsrc_itr.first->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导出口
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_outlet_itr = outs_uni_tex.try_emplace(cur_outlet_name, underlying_rsrc_itr.first, this, uni_tex_usage.get());

	//			if (!cur_outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//资源使用方式存储
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), underlying_rsrc_itr.first);

	//			//[此资源需要在当前pass实体化]
	//			uni_tex_init_list.push_back(underlying_rsrc_itr.first);

	//			return *this;
	//		}

	//		GraphicsPassNode& Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);

	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//首先看imported virtual resource是否已经存在了
	//			auto underlying_rsrc_itr = rg_uni_textures_map.find(underlying_rsrc_name);
	//			//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果

	//			//如果virtual resource不存在，则创建:
	//			if (underlying_rsrc_itr == rg_uni_textures_map.end())
	//			{
	//				const auto temp_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //键值和emplace构造所需的参数
	//				underlying_rsrc_itr = temp_itr.first;
	//			}

	//			//给ItrInRsrcMap增加使用它的pass
	//			underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导出口
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_outlet_itr = outs_uni_tex.try_emplace(cur_outlet_name, underlying_rsrc_itr, this, uni_tex_usage.get());

	//			if (!cur_outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//资源使用方式存储
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), underlying_rsrc_itr);

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		GraphicsPassNode& Out(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkUniversalTexUsageInfoRG> tex_usage)
	//		{
	//			tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);
	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

	//			//从其他pass的outlets中寻找对应资源[texture可以导出后被继续用作attach，attach也可以导出后被继续用作attach]
	//			const auto source_outlet_itr = source_pass.outs_uni_tex.find(source_outlet_name);        //如GbufferPos_GPass_Out

	//			//先找一找源头pass的导出接口是否确实提供了需要的资源
	//			if (source_pass.outs_uni_tex.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
	//			{
	//				const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("isn't provided by the source pass!");
	//				throw std::runtime_error(err_meg.c_str());
	//			}

	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			vrsrc_itr = source_outlet_itr->second.GetUnderlyingRsrcItr();

	//			//给ItrInRsrcMap增加使用它的pass
	//			vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetRsrcAccessType());

	//			//创建当前pass的资源的导出口
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_out_itr = outs_uni_tex.try_emplace(cur_outlet_name, vrsrc_itr, this, tex_usage.get());

	//			if (!cur_out_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//给source outlet增加使用它的pass
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_out_itr.first);

	//			//资源使用方式存储
	//			this->uni_tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

	//			//[此资源不需要在当前pass实体化]
	//			return *this;
	//		}

	//		//************************************************************************************************

	//	public:
	//		void PreExecute() override;
	//		void Execute(VkCommandBuffer cmd_buf) override;

	//		//void ReleaseAndAcquireOperation(
	//		//    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, QueueManager::QueueCapability> &subgraph_execute_on_queue_info,
	//		//    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, size_t> &                       subgraph_execute_on_queue_order,
	//		//    const std::vector<std::pair<RenderGraphV0::GraphicsPassNode *, QueueManager::Queue *>> &    subgraph_queue_pairs,
	//		//    VkCommandBuffer                                                                             cmd_buf)
	//		//{
	//		//	//资源使用方式
	//		//	for (auto &buf_usage : buf_usages)
	//		//	{
	//		//		const auto &root_read_passes_map = buf_usage.buffer->second.root_read_passes_map;        //this is a multimap (key : root pass, value : passes which READ the resource)
	//		//		                                                                                         //首先：找到当前root pass下，所有Read 当前资源的 passes

	//		//		//
	//		//	}
	//		//}
	//	};

	//}        // namespace RenderGraphV0

}        // namespace Anni
