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
	//		//��Դʹ�÷�ʽ
	//		std::vector<VkRsrcUsageInfoRGBuf>          buf_usages;
	//		std::vector<VkRsrcUsageInfoRGUniversalTex> uni_tex_usages;

	//	public:
	//		//��Դ��ʼ���б�
	//		std::vector<std::unordered_map<std::string, VirtualResource<Buffer>>::iterator>    buf_init_list;
	//		std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> uni_tex_init_list;

	//	public:
	//		//�����б��б�
	//		std::vector<std::unordered_map<std::string, VirtualResource<Buffer>>::iterator>    buf_recycle_list;
	//		std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> uni_tex_recycle_list;

	//		//��������ӿ�
	//	public:
	//		std::unordered_map<std::string, RsrcOutlet<Buffer, VkBufUsageInfoRG>>             outs_buf;
	//		std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>> outs_uni_tex;

	//		std::unordered_map<std::string, RsrcInlet<Buffer, VkBufUsageInfoRG>>             ins_buf;
	//		std::unordered_map<std::string, RsrcInlet<VkTexture, VkUniversalTexUsageInfoRG>> ins_uni_tex;

	//		//level dependency
	//		size_t cur_level;

	//		//same queue ͬ��primitive
	//	public:
	//		//cur_pass -> set_event -> wait_event -> target_pass
	//		std::vector<SyncInfoSameQueue<Buffer, Vk::BufSyncInfo>>    buf_syn_infos_tail_same_q;
	//		std::vector<SyncInfoSameQueue<VkTexture, Vk::TexSyncInfo>> uni_tex_syn_infos_tail_same_q;

	//		//source_pass -> set_event -> wait_event -> cur_pass
	//		std::vector<SyncInfoSameQueue<Buffer, Vk::BufSyncInfo>>    buf_syn_infos_head_same_q;
	//		std::vector<SyncInfoSameQueue<VkTexture, Vk::TexSyncInfo>> uni_tex_syn_infos_head_same_q;

	//		//diff queue ͬ��primitive
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
	//		//In��Outֻ������Դ�Ƿ�ᵼ��������passʹ�ã�IN��OUT��������ʱʹ�õ���ص���Դ �ȿ��ܻᱻ��Ҳ���ܻᱻд

	//		//In: ������Դ���ᵼ��������passʹ�ã���ͨ����������dependency
	//		//Out:������Դ �ᵼ��������passʹ��

	//		//TODO����ν���������Ǩ?
	//		//����Buffer��ص������������
	//		//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��

	//		GraphicsPassNode& In(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			//������Դ����Դ��֮����Ҫ��һ������������������TAA������
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);

	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о��״δ�����ע�����ﴴ������������Դ�������費��Ҫʵ�廯��Ҫ���ǲ���EstablishedInSitu

	//			auto rsrc_itr = rg_buffers_map.find(underlying_rsrc_name);
	//			if (rsrc_itr == rg_buffers_map.end())
	//			{
	//				const auto temp_itr = rg_buffers_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_buf);        //��ֵ��emplace����Ԫ������Ĳ���
	//				rsrc_itr = temp_itr.first;
	//			}

	//			const auto inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺���������뵼����

	//			//������ǰpass����Դ�ĵ����
	//			const auto cur_inlet_itr = ins_buf.try_emplace(inlet_name, rsrc_itr, this, buf_usage.get());
	//			if (!cur_inlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Inlet!");
	//			}

	//			//��underlying resource����ʹ������pass
	//			rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//��Դʹ�÷�ʽ�洢��move unique ptr������ı���ײ�����ĵ�ַ
	//			this->buf_usages.emplace_back(std::move(buf_usage), rsrc_itr);

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		//����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
	//		GraphicsPassNode& In(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);

	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

	//			//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
	//			const auto source_outlet_itr = source_pass.outs_buf.find(source_outlet_name);
	//			if (source_pass.outs_buf.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ������ֱ��crash
	//			{
	//				throw std::runtime_error("resource with the name doesn't exist!");
	//			}

	//			//��ItrInRsrcMap ����ʹ������pass
	//			source_outlet_itr->second.GetUnderlyingRsrcItr()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ����
	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");
	//			const auto cur_inlet_itr = ins_buf.try_emplace(cur_inlet_name, source_outlet_itr->second.GetUnderlyingRsrcItr(), this, buf_usage.get());

	//			if (!cur_inlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Inlet!");
	//			}

	//			//��source outlet����ʹ������pass���Լ�ʹ������pass��Ӧ��ʹ�÷�ʽ
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_inlet_itr.first);

	//			//��inlet��ֵ�ṩ����pass
	//			cur_inlet_itr.first->second.AssignProvidingPassAndProvidingOutItr(&source_pass, source_outlet_itr);

	//			//��Դʹ�÷�ʽ�洢
	//			this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetUnderlyingRsrcItr());

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		//buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
	//		GraphicsPassNode& Out(const std::string rsrc_name, Buffer::Descriptor buf_descriptor, const std::function<void(Buffer::Descriptor& desco)>& descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu);
	//			//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������

	//			//��Դ��������ʽΪ����Դ�� + _
	//			const auto rsrc_name = rsrc_name + std::string("_");

	//			//�ô����lambda�����ı�descriptor
	//			descriptor_modifier(buf_descriptor);

	//			//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
	//			const auto underlying_rsrc_itr = rg_buffers_map.try_emplace(rsrc_name, rsrc_name, buf_descriptor);
	//			//ȷ���û�û���ظ����
	//			if (!underlying_rsrc_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��underlying resource����ʹ������pass
	//			underlying_rsrc_itr.first->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ�����
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_out_itr = outs_buf.try_emplace(cur_outlet_name, underlying_rsrc_itr.first, this, buf_usage.get());

	//			if (!cur_out_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exists in Outlet!");
	//			}

	//			//��Դʹ�÷�ʽ�洢
	//			this->buf_usages.emplace_back(std::move(buf_usage), underlying_rsrc_itr.first);

	//			//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
	//			buf_init_list.push_back(underlying_rsrc_itr.first);

	//			return *this;
	//		}

	//		//buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
	//		GraphicsPassNode& Out(const std::string rsrc_name, std::shared_ptr<Buffer> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);

	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//���ȿ�imported virtual resource�Ƿ��Ѿ�������
	//			auto underlying_rsrc_itr = rg_buffers_map.find(underlying_rsrc_name);
	//			//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

	//			//���virtual resource�����ڣ��򴴽�:
	//			if (underlying_rsrc_itr == rg_buffers_map.end())
	//			{
	//				const auto temp_itr = rg_buffers_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_buf);        //��ֵ��emplace��������Ĳ���
	//				underlying_rsrc_itr = temp_itr.first;
	//			}

	//			//��underlying resource����ʹ������pass
	//			underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ�����
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto outlet_itr = outs_buf.try_emplace(cur_outlet_name, underlying_rsrc_itr, this, buf_usage.get());

	//			if (!outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Outlet!");
	//			}

	//			//��Դʹ�÷�ʽ�洢
	//			this->buf_usages.emplace_back(std::move(buf_usage), underlying_rsrc_itr);

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		//����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
	//		GraphicsPassNode& Out(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	//		{
	//			buf_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);

	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

	//			//����һ��  Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ

	//			const auto source_outlet_itr = source_pass.outs_buf.find(source_outlet_name);
	//			if (source_pass.outs_buf.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//			{
	//				throw std::runtime_error("resource with the name doesn't exist in source outlet!");        //��鲻ͨ����ֱ�ӱ����׳��쳣
	//			}

	//			//��ItrInRsrcMap����ʹ������pass
	//			source_outlet_itr->second.GetUnderlyingRsrcItr()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ�����
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_outlet_itr = outs_buf.try_emplace(cur_outlet_name, source_outlet_itr->second.GetUnderlyingRsrcItr(), this, buf_usage.get());

	//			if (!cur_outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist in Outlet!");
	//			}

	//			//source pass�е�outlet���accessing pass
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_outlet_itr.first);

	//			//����Դʹ�÷�ʽ�洢����
	//			this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetUnderlyingRsrcItr());

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		//************************************************************************************************

	//		//����Texture��ص������������
	//		//************************************************************************************************
	//		//��Դ����rendergraph֮�⣬����������ģ�͵�texture��
	//		GraphicsPassNode& In(const std::string rsrc_name, std::vector<std::shared_ptr<VkTexture>> model_textures, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);
	//			//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph

	//			std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> model_tex_rsrc_itrs;
	//			model_tex_rsrc_itrs.reserve(model_textures.size());

	//			for (auto& ptr_tex : model_textures)
	//			{
	//				const auto underlying_rsrc_name = rsrc_name + std::string("_") + ptr_tex->GetTextureName();        //��pass��׺
	//				//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���
	//				auto underlying_rsrc_itr = rg_uni_textures_map.find(underlying_rsrc_name);

	//				if (underlying_rsrc_itr == rg_uni_textures_map.end())
	//				{
	//					const auto temp_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
	//					underlying_rsrc_itr = temp_itr.first;
	//				}

	//				model_tex_rsrc_itrs.push_back(underlying_rsrc_itr);
	//			}

	//			//��ItrInRsrcMap����ʹ������pass���������׶δ���
	//			for (const auto& underlying_rsrc_itr : model_tex_rsrc_itrs)
	//			{
	//				underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());
	//			}

	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺����������inlet��outlet
	//			//������ǰpass����Դ�ĵ����
	//			const auto cur_inlet_in_itr = ins_uni_tex.try_emplace(cur_inlet_name, model_tex_rsrc_itrs, this, uni_tex_usage.get());
	//			if (!cur_inlet_in_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��Դʹ�÷�ʽ�洢
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), std::move(model_tex_rsrc_itrs));

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		GraphicsPassNode& In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);
	//			//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");        //��pass��׺
	//			//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���
	//			auto underlying_rsrc_itr = rg_uni_textures_map.find(underlying_rsrc_name);
	//			if (underlying_rsrc_itr == rg_uni_textures_map.end())
	//			{
	//				const auto temp_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
	//				underlying_rsrc_itr = temp_itr.first;
	//			}

	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺����������inlet��outlet

	//			//������ǰpass����Դ�ĵ����
	//			const auto cur_inlet_in_itr = ins_uni_tex.try_emplace(cur_inlet_name, underlying_rsrc_itr, this, uni_tex_usage.get());
	//			if (!cur_inlet_in_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��ItrInRsrcMap����ʹ������pass���������׶δ���
	//			underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//��Դʹ�÷�ʽ�洢
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), underlying_rsrc_itr);

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		GraphicsPassNode& In(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);

	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

	//			//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
	//			const auto source_outlet_itr = source_pass.outs_uni_tex.find(source_outlet_name);        //��GbufferPos_GPass_Out

	//			if (source_pass.outs_uni_tex.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//			{
	//				const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  isn't provided by the source pass outlet!");
	//				throw std::runtime_error(err_meg.c_str());
	//			}

	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			vrsrc_itr = source_outlet_itr->second.GetUnderlyingRsrcItr();

	//			//��ItrInRsrcMap����ʹ������pass
	//			vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ����
	//			const auto cur_inlet_name = rsrc_name + std::string("_") + this->name + std::string("_In");
	//			const auto cur_inlet_itr = ins_uni_tex.try_emplace(cur_inlet_name, vrsrc_itr, this, uni_tex_usage.get());

	//			if (!cur_inlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��source outlet����ʹ������pass
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_inlet_itr.first);

	//			//��inlet��ֵ�ṩ����pass
	//			cur_inlet_itr.first->second.AssignProvidingPassAndProvidingOutItr(&source_pass, source_outlet_itr);

	//			//��Դʹ�÷�ʽ�洢
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), vrsrc_itr);

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		GraphicsPassNode& Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor& desco)>& descriptor_modifier, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::EstablishedInSitu);
	//			//��ǰ���������������descriptor����Դ�����ڵ�ǰpass������

	//			//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//�ô����lambda�����ı�descriptor
	//			descriptor_modifier(tex_descriptor);

	//			//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
	//			const auto underlying_rsrc_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, tex_descriptor);
	//			//ȷ���û�û���ظ����
	//			if (!underlying_rsrc_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��underlying resource����ʹ������pass
	//			underlying_rsrc_itr.first->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ�����
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_outlet_itr = outs_uni_tex.try_emplace(cur_outlet_name, underlying_rsrc_itr.first, this, uni_tex_usage.get());

	//			if (!cur_outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��Դʹ�÷�ʽ�洢
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), underlying_rsrc_itr.first);

	//			//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
	//			uni_tex_init_list.push_back(underlying_rsrc_itr.first);

	//			return *this;
	//		}

	//		GraphicsPassNode& Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	//		{
	//			uni_tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromOutSide);

	//			const auto underlying_rsrc_name = rsrc_name + std::string("_");

	//			//���ȿ�imported virtual resource�Ƿ��Ѿ�������
	//			auto underlying_rsrc_itr = rg_uni_textures_map.find(underlying_rsrc_name);
	//			//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

	//			//���virtual resource�����ڣ��򴴽�:
	//			if (underlying_rsrc_itr == rg_uni_textures_map.end())
	//			{
	//				const auto temp_itr = rg_uni_textures_map.try_emplace(underlying_rsrc_name, underlying_rsrc_name, ptr_tex);        //��ֵ��emplace��������Ĳ���
	//				underlying_rsrc_itr = temp_itr.first;
	//			}

	//			//��ItrInRsrcMap����ʹ������pass
	//			underlying_rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ�����
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_outlet_itr = outs_uni_tex.try_emplace(cur_outlet_name, underlying_rsrc_itr, this, uni_tex_usage.get());

	//			if (!cur_outlet_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��Դʹ�÷�ʽ�洢
	//			this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), underlying_rsrc_itr);

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//			return *this;
	//		}

	//		GraphicsPassNode& Out(const std::string rsrc_name, PassNode& source_pass, std::unique_ptr<VkUniversalTexUsageInfoRG> tex_usage)
	//		{
	//			tex_usage->SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin::FromSourcePass);
	//			const auto source_outlet_name = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

	//			//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
	//			const auto source_outlet_itr = source_pass.outs_uni_tex.find(source_outlet_name);        //��GbufferPos_GPass_Out

	//			//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
	//			if (source_pass.outs_uni_tex.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//			{
	//				const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("isn't provided by the source pass!");
	//				throw std::runtime_error(err_meg.c_str());
	//			}

	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			vrsrc_itr = source_outlet_itr->second.GetUnderlyingRsrcItr();

	//			//��ItrInRsrcMap����ʹ������pass
	//			vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetRsrcAccessType());

	//			//������ǰpass����Դ�ĵ�����
	//			const auto cur_outlet_name = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//			const auto cur_out_itr = outs_uni_tex.try_emplace(cur_outlet_name, vrsrc_itr, this, tex_usage.get());

	//			if (!cur_out_itr.second)
	//			{
	//				throw std::runtime_error("resource with the same name already exist!");
	//			}

	//			//��source outlet����ʹ������pass
	//			source_outlet_itr->second.AddAccessingPassAndItr(this, cur_out_itr.first);

	//			//��Դʹ�÷�ʽ�洢
	//			this->uni_tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

	//			//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
	//		//	//��Դʹ�÷�ʽ
	//		//	for (auto &buf_usage : buf_usages)
	//		//	{
	//		//		const auto &root_read_passes_map = buf_usage.buffer->second.root_read_passes_map;        //this is a multimap (key : root pass, value : passes which READ the resource)
	//		//		                                                                                         //���ȣ��ҵ���ǰroot pass�£�����Read ��ǰ��Դ�� passes

	//		//		//
	//		//	}
	//		//}
	//	};

	//}        // namespace RenderGraphV0

}        // namespace Anni
