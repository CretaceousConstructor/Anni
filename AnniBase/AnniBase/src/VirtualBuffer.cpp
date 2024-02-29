#include "VirtualBuffer.h"


Anni::RenderGraphV1::VirtualBuffer::Handle::Handle() = default;

Anni::RenderGraphV1::VirtualBuffer::Handle::Handle(uint64_t handle_): handle(handle_)
{
}

Anni::RenderGraphV1::VirtualBuffer::VirtualBuffer(std::string name_, std::shared_ptr<Buffer> ptr_rsrc_):
	IVirtualResource(name_, VRsrcType::Imported), p_rsrc(ptr_rsrc_), descriptor(std::nullopt)
{
}

Anni::RenderGraphV1::VirtualBuffer::VirtualBuffer(std::string name_, Buffer::Descriptor descriptor_):
	IVirtualResource(name_, VRsrcType::Established), descriptor(descriptor_)
{
}

VkBufferMemoryBarrier2 Anni::RenderGraphV1::VirtualBuffer::GetBufBarrier(const Anni::BufSyncInfo& source_syn,
                                                                         const Anni::BufSyncInfo& target_sync) const
{
	//    VkStructureType          sType;
	//    const void*              pNext;
	//    VkPipelineStageFlags2    srcStageMask;
	//    VkAccessFlags2           srcAccessMask;
	//    VkPipelineStageFlags2    dstStageMask;
	//    VkAccessFlags2           dstAccessMask;
	//    uint32_t                 srcQueueFamilyIndex;
	//    uint32_t                 dstQueueFamilyIndex;
	//    VkBuffer                 buffer;
	//    VkDeviceSize             offset;
	//    VkDeviceSize             size;
	//} VkBufferMemoryBarrier2;

	const Vk::BufferSubRange sor_sub_range = source_syn.buf_subrange.value_or(Vk::BufferSubRange{
		.offset = 0, .size = VK_WHOLE_SIZE
	});
	const Vk::BufferSubRange tar_sub_range = target_sync.buf_subrange.value_or(Vk::BufferSubRange{
		.offset = 0, .size = VK_WHOLE_SIZE
	});


	ASSERT_WITH_MSG(tar_sub_range.offset == sor_sub_range.offset, "subranges diverge.");
	ASSERT_WITH_MSG(tar_sub_range.size == sor_sub_range.size, "subranges diverge.");

	vk::BufferMemoryBarrier2 result{};
	result.srcStageMask = source_syn.stage_mask;
	result.srcAccessMask = source_syn.access_mask;
	result.dstStageMask = target_sync.stage_mask;
	result.dstAccessMask = target_sync.access_mask;
	result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	result.buffer = p_rsrc->GetGPUBuffer();
	result.offset = sor_sub_range.offset;
	result.size = sor_sub_range.size;

	return result;
}
