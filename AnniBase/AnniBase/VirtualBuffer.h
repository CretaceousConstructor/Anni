#pragma once
#include "IVirtualResource.h"
#include "Buffer.h"



namespace Anni::RenderGraphV1
{


	class VirtualBuffer : public IVirtualResource
	{
	public:

		VirtualBuffer(std::string name_, std::shared_ptr<Buffer> ptr_rsrc_) :
			IVirtualResource(name_, VRsrcType::Imported), p_rsrc(ptr_rsrc_), descriptor(std::nullopt)
		{
		}

		VirtualBuffer(std::string name_, Buffer::Descriptor descriptor_) :
			IVirtualResource(name_, VRsrcType::Established), descriptor(descriptor_)
		{
		}

		VirtualBuffer() = delete;
		VirtualBuffer(const VirtualBuffer&) = delete;
		VirtualBuffer& operator=(const VirtualBuffer&) = delete;
		VirtualBuffer(VirtualBuffer&&) = delete;
		VirtualBuffer& operator=(VirtualBuffer&&) = delete;
		~VirtualBuffer() = default;



		VkBufferMemoryBarrier2 GetBufBarrier(const Anni::BufSyncInfo& source_syn, const Anni::BufSyncInfo& target_sync)
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

			Vk::BufferSubRange sor_sub_range = source_syn.buf_subrange.value_or(Vk::BufferSubRange{ .offset = 0, .size = VK_WHOLE_SIZE });
			Vk::BufferSubRange tar_sub_range = target_sync.buf_subrange.value_or(Vk::BufferSubRange{ .offset = 0, .size = VK_WHOLE_SIZE });


			VULKAN_HPP_ASSERT(tar_sub_range.offset == sor_sub_range.offset, "subranges diverge.");
			VULKAN_HPP_ASSERT(tar_sub_range.size   == sor_sub_range.size,   "subranges diverge.");

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


		std::shared_ptr<Buffer>                    p_rsrc;
		std::optional<Buffer::Descriptor>          descriptor;
	};





}
