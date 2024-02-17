#pragma once
#include "VkRenderpassManager.h"
#include "VkTexture.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
//
//namespace RenderGraphV0
//{
//	class PassNode;
//}
//
//
//
//template <typename RESOURCE>
//class VirtualResource
//{
//private:
//	using ReadPass = RenderGraphV0::PassNode;
//	using RootPass = RenderGraphV0::PassNode;
//
//public:
//	enum class RsrcType
//	{
//		Imported,
//		Established
//	};
//
//public:
//	VirtualResource(std::string name_, std::shared_ptr<RESOURCE> ptr_rsrc_) :
//
//		name(name_), ptr_rsrc(ptr_rsrc_), descriptor(std::nullopt), rsrc_type(RsrcType::Imported)
//	{
//	}
//
//	VirtualResource(std::string name_, typename RESOURCE::Descriptor descriptor_) :
//		name(name_), ptr_rsrc(nullptr), descriptor(descriptor_), rsrc_type(RsrcType::Established)
//	{
//	}
//
//	VirtualResource() = delete;
//
//	VirtualResource(const VirtualResource&) = delete;
//	VirtualResource& operator=(const VirtualResource&) = delete;
//
//	VirtualResource(VirtualResource&&) = delete;
//	VirtualResource& operator=(VirtualResource&&) = delete;
//
//	~VirtualResource() = default;
//
//public:
//	void Actualize(VkRenderpassManager&);
//	void Recycle(VkRenderpassManager&);
//	void AssignReadPassesMap(std::unordered_multimap<RootPass*, ReadPass*> root_read_passes_map);
//
//	VkBufferMemoryBarrier2 GetBufBarrier(const Vk::BufSyncInfo& source_syn, const Vk::BufSyncInfo& target_sync);
//	VkImageMemoryBarrier2  GetTexBarrier(const Vk::TexSyncInfo& source_syn, const Vk::TexSyncInfo& target_sync);
//
//public:
//	std::string                                  name;
//	std::shared_ptr<RESOURCE>                    ptr_rsrc;
//	std::optional<typename RESOURCE::Descriptor> descriptor;
//	VirtualResource::RsrcType                    rsrc_type;
//
//	std::map<size_t, std::vector<RenderGraphV0::PassNode*>>        level_to_passes_attached_to;
//	std::map<size_t, std::set<DeviceManager::VkExecutionQueue*>>   level_to_unique_queues;
//	std::vector<std::pair<RenderGraphV0::PassNode*, Vk::AccessType>> passes_access_this_rsrc;
//
//public:
//	//which passes read this rsrc, and these reading passes's root passes are used as the keys of unordered multi map.
//	std::unordered_multimap<RootPass*, ReadPass*> root_read_passes_map;
//};
//
////
////
////Rsrc Actualization spcialization
//
//template <>
//inline void VirtualResource<VkTexture>::Actualize(VkRenderpassManager& renderpass_manager)
//{
//	if (!descriptor.has_value())
//	{
//		throw std::runtime_error("No descriptor is provided.");
//	}
//
//	const auto& dis = descriptor.value();
//	ptr_rsrc = renderpass_manager.GetTextureFactory().ProduceUnfilledTexture(*dis.tex_img_CI, dis.sampler_CI, dis.img_view_CI);
//}
//
//template <>
//inline void VirtualResource<Buffer>::Actualize(VkRenderpassManager& renderpass_manager)
//{
//	if (!descriptor.has_value())
//	{
//		throw std::runtime_error("No descriptor is provided.");
//	}
//
//	const auto& dis = descriptor.value();
//
//	ptr_rsrc = renderpass_manager.GetBufferFactory().ProduceBuffer(dis.buffer_size, descriptor.value());
//}
//
//template <>
//inline void VirtualResource<VkTexture>::Recycle(VkRenderpassManager& renderpass_manager)
//{
//	ptr_rsrc.reset();
//}
//
//template <>
//inline void VirtualResource<Buffer>::Recycle(VkRenderpassManager& renderpass_manager)
//{
//	ptr_rsrc.reset();
//}
//
////template <>
////inline void VirtualResource<VkTexture>::InsertSyncIntoCmdBuf(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync)
////{
////	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
////	    .srcAccessMask = source_syn.access_mask,
////	    .srcStageMask  = source_syn.stage_mask,
////	    .oldLayout     = source_syn.layout_inpass,
////
////	    .dstAccessMask = target_sync.access_mask,
////	    .dstStageMask  = target_sync.stage_mask,
////	    .newLayout     = target_sync.layout_inpass,
////	    //TODO: sub ranges
////	    .subresource_range = std::nullopt,
////	};
////	ptr_rsrc->InsertImageMemoryBarrier(cmd_buf, image_memory_barrier_enhanced);
////}
//
//template <typename RESOURCE>
//inline void VirtualResource<RESOURCE>::AssignReadPassesMap(std::unordered_multimap<RootPass*, ReadPass*> root_read_passes_map)
//{
//	root_read_passes_map = std::move(root_read_passes_map);
//}
//
//template <>
//inline VkBufferMemoryBarrier2 VirtualResource<Buffer>::GetBufBarrier(const Vk::BufSyncInfo& source_syn, const Vk::BufSyncInfo& target_sync)
//{
//	//    VkStructureType          sType;
//	//    const void*              pNext;
//	//    VkPipelineStageFlags2    srcStageMask;
//	//    VkAccessFlags2           srcAccessMask;
//	//    VkPipelineStageFlags2    dstStageMask;
//	//    VkAccessFlags2           dstAccessMask;
//	//    uint32_t                 srcQueueFamilyIndex;
//	//    uint32_t                 dstQueueFamilyIndex;
//	//    VkBuffer                 buffer;
//	//    VkDeviceSize             offset;
//	//    VkDeviceSize             size;
//	//} VkBufferMemoryBarrier2;
//
//	if (source_syn.offset != target_sync.offset ||
//		source_syn.size != target_sync.size)
//	{
//		throw std::runtime_error("source and target sync info differ!");
//	}
//
//	VkBufferMemoryBarrier2 result{
//		.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
//		.pNext = VK_NULL_HANDLE,
//		.srcStageMask = source_syn.stage_mask,
//		.srcAccessMask = source_syn.access_mask,
//		.dstStageMask = target_sync.stage_mask,
//		.dstAccessMask = target_sync.access_mask,
//		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.buffer = ptr_rsrc->GetGPUBuffer(),
//		.offset = source_syn.offset,
//		.size = source_syn.size };
//
//	return result;
//}
//
//template <>
//inline VkImageMemoryBarrier2 VirtualResource<VkTexture>::GetTexBarrier(const Vk::TexSyncInfo& source_syn, const Vk::TexSyncInfo& target_sync)
//{
//	//    VkStructureType            sType;
//	//    const void*                pNext;
//	//    VkPipelineStageFlags2      srcStageMask;
//	//    VkAccessFlags2             srcAccessMask;
//	//    VkPipelineStageFlags2      dstStageMask;
//	//    VkAccessFlags2             dstAccessMask;
//	//    VkImageLayout              oldLayout;
//	//    VkImageLayout              newLayout;
//	//    uint32_t                   srcQueueFamilyIndex;
//	//    uint32_t                   dstQueueFamilyIndex;
//	//    VkImage                    image;
//	//    VkImageSubresourceRange    subresourceRange;
//	//} VkImageMemoryBarrier2;
//
//	VkImageSubresourceRange result_range;
//
//	//typedef struct VkImageSubresourceRange {
//	//    VkImageAspectFlags    aspectMask;
//	//    uint32_t              baseMipLevel;
//	//    uint32_t              levelCount;
//	//    uint32_t              baseArrayLayer;
//	//    uint32_t              layerCount;
//	//} VkImageSubresourceRange;
//
//	if (!source_syn.img_subrange && !target_sync.img_subrange)
//	{
//		result_range = descriptor.value().img_view_CI.value().subresourceRange;
//	}
//	else if (source_syn.img_subrange && target_sync.img_subrange)
//	{
//		result_range = source_syn.img_subrange.value();
//	}
//	else
//	{
//		throw std::runtime_error("source and target sync info differ!");
//	}
//
//	VkImageMemoryBarrier2 result{
//		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
//		.pNext = VK_NULL_HANDLE,
//		.srcStageMask = source_syn.stage_mask,
//		.srcAccessMask = source_syn.access_mask,
//		.dstStageMask = target_sync.stage_mask,
//		.dstAccessMask = target_sync.access_mask,
//		.oldLayout = source_syn.layout_inpass,
//		.newLayout = target_sync.layout_inpass,
//		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.image = ptr_rsrc->GetTextureRawImage(),
//		.subresourceRange = result_range };
//
//	return result;
//}
//
////template <>
////inline void VirtualResource<Buffer>::InsertSyncIntoCmdBuf(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync)
////{
////	//TODO:
////}
