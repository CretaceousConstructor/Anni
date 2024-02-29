#include "VirtualTexture.h"


Anni::RenderGraphV1::VirtualTexture::Handle::Handle() = default;

Anni::RenderGraphV1::VirtualTexture::Handle::Handle(uint64_t handle_): handle(handle_)
{
			  
}

Anni::RenderGraphV1::VirtualTexture::VirtualTexture(std::string name_,
	std::vector<std::shared_ptr<VkTexture>>* const p_rsrcs_):
	IVirtualResource(name_, VRsrcType::Imported), descriptor(std::nullopt), p_rsrcs(p_rsrcs_)
{
}

Anni::RenderGraphV1::VirtualTexture::VirtualTexture(std::string name_, VkTexture::Descriptor descriptor_):
	IVirtualResource(name_, VRsrcType::Established), descriptor(descriptor_)
{
}

Anni::RenderGraphV1::VirtualTexture::VirtualTexture(std::string name_, std::shared_ptr<VkTexture> ptr_rsrc_):
	IVirtualResource(name_, VRsrcType::Imported), p_rsrc(ptr_rsrc_), descriptor(std::nullopt)
{
}

vk::ImageMemoryBarrier2
Anni::RenderGraphV1::VirtualTexture::GetTexBarrier(
  const Anni::ImgSyncInfo& source_syn,
  const Anni::ImgSyncInfo& target_sync) const
{
  //    VkStructureType            sType;
  //    const void*                pNext;
  //    VkPipelineStageFlags2      srcStageMask;
  //    VkAccessFlags2             srcAccessMask;
  //    VkPipelineStageFlags2      dstStageMask;
  //    VkAccessFlags2             dstAccessMask;
  //    VkImageLayout              oldLayout;
  //    VkImageLayout              newLayout;
  //    uint32_t                   srcQueueFamilyIndex;
  //    uint32_t                   dstQueueFamilyIndex;
  //    VkImage                    image;
  //    VkImageSubresourceRange    subresourceRange;
  //} VkImageMemoryBarrier2;

  vk::ImageSubresourceRange result_range{};

  // typedef struct VkImageSubresourceRange {
  //     VkImageAspectFlags    aspectMask;
  //     uint32_t              baseMipLevel;
  //     uint32_t              levelCount;
  //     uint32_t              baseArrayLayer;
  //     uint32_t              layerCount;
  // } VkImageSubresourceRange;

  if (!source_syn.img_subrange && !target_sync.img_subrange) {
    result_range = descriptor.value().img_view_CI.value().subresourceRange;
  } else if (source_syn.img_subrange && target_sync.img_subrange) {
    assert(source_syn.img_subrange.value() == target_sync.img_subrange.value());
    result_range = source_syn.img_subrange.value();
  } else {
    ASSERT_WITH_MSG(false, "source and target sync info differ!");
  }

  ASSERT_WITH_MSG(p_rsrc || p_rsrcs, "no resources provided!");

  vk::ImageMemoryBarrier2 result{};
  result.srcStageMask = source_syn.stage_mask;
  result.srcAccessMask = source_syn.access_mask;
  result.dstStageMask = target_sync.stage_mask;
  result.dstAccessMask = target_sync.access_mask;
  result.oldLayout = source_syn.layout_inpass;
  result.newLayout = target_sync.layout_inpass;
  result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  result.image = p_rsrc->GetTextureRawImage();
  result.subresourceRange = result_range;

  return result;
}
