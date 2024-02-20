#include "BufViewWrapper.h"


namespace Anni
{
	vk::BufferViewCreateInfo CI::PopulateUniformBufViewCI(const vk::Format buf_format)
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

		vk::BufferViewCreateInfo result{};
		result.setFormat(buf_format);
		result.setRange(vk::WholeSize);
		return result;
	}

	BufViewWrapper::BufViewWrapper(DeviceManager& device_man_, vk::BufferViewCreateInfo buf_view_CI_) :
		device_manager(device_man_), buf_view_CI(buf_view_CI_)
	{
		buffer_view = device_manager.GetLogicalDevice().createBufferView(buf_view_CI);
	}


	BufViewWrapper::~BufViewWrapper()
	{
		device_manager.GetLogicalDevice().destroyBufferView(buffer_view);
	}

	vk::BufferView& BufViewWrapper::GetRawBufView()
	{
		return buffer_view;
	}

}        // namespace Anni
