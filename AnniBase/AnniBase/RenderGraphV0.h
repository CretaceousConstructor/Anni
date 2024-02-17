#pragma once
//#include "DeviceManager.h"
//#include "RenderGraphPass.h"
//#include "Vk.h"
//#include "VkRenderpassBaseRG.h"
//#include "VkRenderpassManager.h"
//#include "VkRsrcState.h"
//#include "VkRsrcUsageInfoRG.h"
//
//#include <algorithm>
//#include <array>
//#include <functional>
//#include <ranges>
//namespace Anni
//{
//	namespace RenderGraphV0
//	{
//		//class GraphEdge
//		//{
//		//  public:
//		//	GraphEdge(PassNode *f, PassNode *t) :
//		//	    from(f), to(t)
//		//	{
//		//	}
//		//
//		//	PassNode *from;
//		//	PassNode *to;
//		//};
//		//
//		//class GraphNode
//		//{
//		//  public:
//		//	GraphNode(PassNode *node_) :
//		//	    curr_pass(node_)
//		//	{
//		//	}
//		//	void AddEdge(GraphEdge edge)
//		//	{
//		//		edges.push_back(edge);
//		//	}
//		//	PassNode *             curr_pass;
//		//	std::vector<GraphEdge> edges;
//		//};
//
//		;
//
//		class PassPair
//		{
//		public:
//			PassPair(
//				PassNode* pass0_,
//				PassNode* pass1_) :
//				pass0(pass0_), pass1(pass1_)
//			{
//			}
//
//			// Equality operator
//			bool operator==(const PassPair& other) const
//			{
//				return
//					((pass0 == other.pass0) && (pass1 == other.pass1)) ||
//					((pass0 == other.pass1) && (pass0 == other.pass0));
//			}
//
//			// Hash function
//			struct Hash
//			{
//				size_t operator()(const PassPair& obj) const
//				{
//					// 使用哈希组合函数，确保指针的顺序不影响结果
//					std::hash<PassNode*> hasher;
//
//					// 对两个指针进行哈希，使用异或操作
//					size_t hash1 = hasher(obj.pass0);
//					size_t hash2 = hasher(obj.pass1);
//
//					return hash1 ^ hash2;
//				}
//			};
//
//		public:
//			PassNode* pass0;
//			PassNode* pass1;
//		};
//
//		class DependencyGraph
//		{
//		public:
//			DependencyGraph(const DeviceManager& device_man_, VkRenderpassManager& renderpass_manager_);
//
//			DependencyGraph() = delete;
//			~DependencyGraph() = default;
//
//			DependencyGraph(const DependencyGraph&) = delete;
//			DependencyGraph& operator=(const DependencyGraph&) = delete;
//			DependencyGraph(DependencyGraph&&) = delete;
//			DependencyGraph& operator=(DependencyGraph&&) = delete;
//
//		public:
//			VkAttachmentInfo GetAttachmentRsrcIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//			VkAttachmentInfo GetAttachmentRsrcOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//
//			VkTexUsageInfo GetTextureRsrcIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//			VkTexUsageInfo GetTextureRsrcOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//
//			VkUniBufUsageInfo GetBufferRsrcIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//			VkUniBufUsageInfo GetBufferRsrcOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//
//			void StateChangeNoNeedCmdRecordingAttachmentOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//			void StateChangeNoNeedCmdRecordingAttachmentIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//
//			void StateChangeNoNeedCmdRecordingTextureIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//			void StateChangeNoNeedCmdRecordingTextureOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//
//			void StateChangeNoNeedCmdRecordingBufferIn(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//			void StateChangeNoNeedCmdRecordingBufferOut(std::string rsrc_name, PassNode* pass, uint32_t usage_index = 0);
//
//			GraphicsPassNode& AddGfxPassNode(std::string name, std::shared_ptr<VkRenderpassBaseRG> renderpass_ptr);
//
//			bool Compile(const DeviceManager& device_man);
//			bool Execute(VkCommandBuffer cmb);
//			bool ExecutRenderGraphV0(VkCommandBuffer cmb);
//			bool ParallelExecuteRenderGraphV0(const DeviceManager& device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore);
//			bool ParallelExecuteRenderGraphV0V0(const DeviceManager& device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore);
//
//			void CmdBufExecuteOnQueue(PassNode* p_pass, VkCommandBuffer cmd_buf, DeviceManager::VkExecutionQueue* execution_queue, VkSemaphore image_available_semaphore);
//
//		private:
//			void GeneratePassNodeDepen();
//			void GeneratePassNodeDepenFromBuf(PassNode& pass_node);
//			void GeneratePassNodeDepenFromTex(PassNode& pass_node);
//			void TopologicalDfsSort();
//			void SortPassesAccessingRsrc();
//			void DependencyLevelsGeneration();
//			void GPUWorkDistribution();
//			void SyncPrimitivesAquisition();
//			void SortPassesAtSameLevel();
//			void SyncPrimitivesInsertion();
//
//			//void      SortAccessingPasses(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const;
//			void ExecutePass(VkCommandBuffer cmd_buf, PassNode* pass);
//			bool SortByTopologicallySortedPasses(const PassNode* a, const PassNode* b, const std::vector<PassNode*>& order);
//
//		public:
//			void   VisitNode(size_t index, PassNode* cur_node_pass, std::vector<bool>& visited, std::vector<bool>& on_stack, std::vector<PassNode*>& topologically_sorted_nodes);
//			size_t FindIndexInPassNodeArray(PassNode* node_pass, const std::vector<std::unique_ptr<GraphicsPassNode>>& all_nodes);
//
//		public:
//			//使用unordered map，迭代器除非指向的是被删除的元素，否则迭代器[ 不会失效 ]
//			std::unordered_map<std::string, VirtualResource<Buffer>>    buffers_map;
//			std::unordered_map<std::string, VirtualResource<VkTexture>> uni_textures_map;
//			std::vector<std::unique_ptr<GraphicsPassNode>>              gfx_pass_nodes;
//
//			std::unordered_map<DeviceManager::VkExecutionQueue*, std::list<PassNode*>>         queue_to_passes_on_it;
//			std::unordered_map<PassNode*, std::pair<DeviceManager::VkExecutionQueue*, size_t>> pass_to_queue_info;
//			size_t                                                                               max_level = 0;
//			std::vector<PassNode*>                                                              topologically_sorted_nodes;
//
//		private:
//			//FOR EVERY pass pair, we assign it an event, in order to do sync within a same queue;
//			std::unordered_map<PassPair, VkEvent>     same_queue_sync_event;
//			std::unordered_map<PassPair, VkSemaphore> diff_queue_sync_sema;
//			std::unordered_map<size_t, VkSemaphore>   same_level_sync_sema; 
//
//		private:
//			const DeviceManager& device_man;
//			VkRenderpassManager& renderpass_manager;
//
//		private:
//			//head数组
//			//边数组
//			//std::vector<GraphNode> heads;
//			//std::vector<GraphEdge> edges;
//		};
//
//	}        // namespace RenderGraphV0
//}        // namespace Anni
