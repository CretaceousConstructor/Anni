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
#include "PresentPass.h"
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
			DescriptorSetAllocatorGrowable& descriptor_allocator_,
			VkTimelineSemPoolUnsafe& semaphore_pool_,
			BufferFactory& buf_fac_,
			VkTextureFactory& tex_fac_,
			VkShaderFactory& shader_fac_,
			GFXPipelineCIBuilder& gfx_pipelineCI_builder_);

	public:
		template <typename P>
		P& AddGfxPassNode(std::string name);

		void SetCurFrameAndImgIndex(uint64_t cur_frame_, uint32_t img_index_);


		void Compile();
		void CmdBufRecordingAndExecuting(
			uint32_t img_index,
			uint32_t cur_frame,
			vk::Semaphore frame_number_semaphore);

	public:
		DependencyGraph() = delete;
		~DependencyGraph() = default;

		DependencyGraph(const DependencyGraph&) = delete;
		DependencyGraph& operator=(const DependencyGraph&) = delete;
		DependencyGraph(DependencyGraph&&) = delete;
		DependencyGraph& operator=(DependencyGraph&&) = delete;

	private:

		void CmdBufExecuteOnQueue(
			GraphicsPassNode* p_pass,
			uint32_t cur_frame,
			vk::CommandBuffer cmd_buf,
			Queue* execution_queue,
			VkSemaphore frame_num_sem,
			std::vector<vk::SemaphoreSubmitInfo>& wait_sem_submit_infos,
			std::vector<vk::SemaphoreSubmitInfo>& signal_sem_submit_infos
		);

		void GeneratePassNodeDepen();
		void GeneratePassNodeDepen(GraphicsPassNode& pass_node);
		void DependencyProducedFromBufInlet(GraphicsPassNode& pass_node);
		void DependencyProducedFromBufOutlet(GraphicsPassNode& pass_node);
		void DependencyProducedFromTexInlet(GraphicsPassNode& pass_node);
		void DependencyProducedFromTexOutlet(GraphicsPassNode& pass_node);

		void TopologicalDFSSort();
		void VisitNode(size_t cur_index, GraphicsPassNode* cur_node_pass, std::vector<bool>& visited, std::vector<bool>& on_stack, std::vector<GraphicsPassNode*>& topologically_sorted_nodes);
		size_t FindIndexInPassNodeArray(GraphicsPassNode* node_pass, const std::vector<std::unique_ptr<GraphicsPassNode>>& all_nodes);

		void DependencyLevelsGeneration();

		void GPUWorkDistribution();
		Anni::Queue& GetSuitableQueue(GraphicsPassNode& pass);

		void SyncPrimitivesAquisition();
		void MarkLayersMultiAccessSameRsrc();

		void PartitionPassesAtTheSameLevel();
		void SortPassesAccessingRsrc();
		bool SortByTopologicallySortedPasses(const GraphicsPassNode* a, const GraphicsPassNode* b, const std::vector<GraphicsPassNode*>& order);

		void SyncPrimitivesInsertion();
		void SyncPrimitivesInsertionInletBuffer(GraphicsPassNode* const cur_pass);
		void SyncPrimitivesInsertionOutletBuffer(GraphicsPassNode* const cur_pass);
		void SyncPrimitivesInsertionInletTexture(GraphicsPassNode* const cur_pass);
		void SyncPrimitivesInsertionOutletTexture(GraphicsPassNode* const cur_pass);


	private:
		//全知全能的虚拟资源管理者
		std::list<VirtualBuffer>  rg_buffers;
		std::list<VirtualTexture> rg_textures;
		//当前帧所有的pass
		std::vector<std::unique_ptr<GraphicsPassNode>> pass_nodes;
		//拓扑排序以后的所有passnode	
		std::vector<GraphicsPassNode*>                 topologically_sorted_nodes;

		//当前帧的所有pass最大层级
		using Level = size_t;
		Level         max_level{ 0 };
		std::unordered_map<Level, bool>                if_level_accesses_same_rsrc_multimes;
		std::map<Level, std::vector<GraphicsPassNode*>> level_to_all_passes_attached;

		//pass在queue上的执行信息
		using MonotonicalAssignedIndex = size_t;
		std::unordered_map<Queue*, std::vector<GraphicsPassNode*>>                         queue_to_passes_on_it;
		std::unordered_map<GraphicsPassNode*, std::pair<Queue*, MonotonicalAssignedIndex>> pass_to_queue_info;

		//同步信息
		using DependencyLevel = size_t;
		std::unordered_map<PassNodePair, std::shared_ptr<TimelineSemWrapper>,  PassNodePair::Hash> diff_queue_sync_sema;

		//swap image同步信息
		std::list<std::pair< std::vector<std::shared_ptr<BinarySemWrapper>>, uint64_t>> vec_swap_imgs_finshed_using; //这里用list防止容器失效



	private:
		uint64_t cur_frame;
		uint32_t img_index;

	private:
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		QueueManager& queue_manager;
		DescriptorLayoutManager& descriptor_set_layout_manager;

		DescriptorSetAllocatorGrowable& descriptor_allocator;
		VkTimelineSemPoolUnsafe semaphore_pool;

		BufferFactory& buf_fac;
		VkTextureFactory& tex_fac;
		VkShaderFactory& shader_fac;

		GFXPipelineCIBuilder& gfx_pipelineCI_builder;

	};



	template <>
	inline DeferedGeometryPass& DependencyGraph::AddGfxPassNode<DeferedGeometryPass>(std::string name)
	{
		auto gfx_node =
			std::make_unique<DeferedGeometryPass>(
				name,
				device_manager,
				swapchain_manager,
				descriptor_set_layout_manager,
				shader_fac,
				descriptor_allocator,
				this->rg_buffers,
				this->rg_textures,
				gfx_pipelineCI_builder
			);
		pass_nodes.push_back(std::move(gfx_node));
		auto& result = *pass_nodes.back();
		DeferedGeometryPass& derived_pass = static_cast<DeferedGeometryPass&>(result);

		return derived_pass;
	}


	template <>
	inline PresentPass& DependencyGraph::AddGfxPassNode<PresentPass>(std::string name)
	{
		auto gfx_node =
			std::make_unique<PresentPass>(
				name,
				device_manager,
				swapchain_manager,
				descriptor_set_layout_manager,
				shader_fac,
				descriptor_allocator,
				this->rg_buffers,
				this->rg_textures
			);
		pass_nodes.push_back(std::move(gfx_node));
		auto& result = *pass_nodes.back();
		PresentPass& derived_pass = static_cast<PresentPass&>(result);

		return derived_pass;
	}






}
