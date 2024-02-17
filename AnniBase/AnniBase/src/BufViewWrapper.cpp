#include "BufViewWrapper.h"


namespace Anni
{
	VkBufferViewCreateInfo CI::PopulateUniformBufViewCI(const VkFormat buf_format)
	{
		//typedef struct VkBufferViewCreateInfo {
		//    VkStructureType            sType;
		//    const void*                pNext;
		//    VkBufferViewCreateFlags    flags;
		//    VkBuffer                   buffer;
		//    VkFormat                   format;
		//    VkDeviceSize               offset;
		//    VkDeviceSize               range;
		//} VkBufferViewCreateInfo;

		VkBufferViewCreateInfo result{
			.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = Vk::NoneFlag,
			.buffer = VK_NULL_HANDLE,
			.format = buf_format,
			.offset = 0,
			.range = VK_WHOLE_SIZE };
		return result;
	}

	BufViewWrapper::BufViewWrapper(DeviceManager& device_man_, VkBufferViewCreateInfo buf_view_CI_) :
		device_manager(device_man_), buf_view_CI(buf_view_CI_)
	{
		VK_CHECK_RESULT(vkCreateBufferView(device_manager.GetLogicalDevice(), &buf_view_CI, nullptr, &buffer_view))
	}

	BufViewWrapper::~BufViewWrapper()
	{
		vkDestroyBufferView(device_manager.GetLogicalDevice(), buffer_view, nullptr);
	}

	VkBufferView BufViewWrapper::GetRawBufView()
	{
		return buffer_view;
	}

}        // namespace Anni
