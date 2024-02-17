#include "ImageFactory.h"
namespace Anni
{
	ImageFactory::ImageFactory(GraphicsComponent& _gfx) :
		gfx(_gfx),
		device_manager(gfx.DeviceMan()),
		swapchain_manager(gfx.SwapchainMan())
	{
	}

	std::shared_ptr<GeneralPurposeImageReFac> ImageFactory::ProduceImage(ImageCIEnhanced img_CI) const
	{
		const auto image = BuildImage(img_CI);
		const auto image_mem = CreateAndBindMemory(img_CI, image);

		auto result = std::make_shared<GeneralPurposeImageReFac>(gfx, image, image_mem, img_CI);

		return result;
	}

	VkImage ImageFactory::BuildImage(ImageCIEnhanced& vk_image_CI) const
	{
		VkImage temp_image;
		if (vkCreateImage(device_manager.GetLogicalDevice(), &vk_image_CI.vk_image_CI, nullptr, &temp_image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}
		return temp_image;
	}

	VkDeviceMemory ImageFactory::CreateAndBindMemory(ImageCIEnhanced& vk_image_CI, VkImage temp_image) const
	{
		VkDeviceMemory temp_image_mem;
		vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_image, &vk_image_CI.mem_req);

		vk_image_CI.mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vk_image_CI.mem_alloc_info.allocationSize = vk_image_CI.mem_req.size;
		vk_image_CI.mem_alloc_info.memoryTypeIndex = DeviceManager::FindMemoryType(vk_image_CI.mem_req.memoryTypeBits, vk_image_CI.mem_prop, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index

		VK_CHECK_RESULT(vkAllocateMemory(device_manager.GetLogicalDevice(), &vk_image_CI.mem_alloc_info, nullptr, &temp_image_mem));
		vkBindImageMemory(device_manager.GetLogicalDevice(), temp_image, temp_image_mem, 0);        //把两者联系起来

		return temp_image_mem;
	}

	//void ImageFactory::TransitionImageLayout(ImageCIReFac &para_pack, const std::shared_ptr<ImageBaseReFac> &result)
	//{
	//	if (para_pack.default_layout_right_aft_creation == VK_IMAGE_LAYOUT_UNDEFINED)
	//	{
	//		return;
	//	}
	//	else if (para_pack.default_layout_right_aft_creation == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	//	{
	//		VkImageSubresourceRange subresource_range;
	//		subresource_range.baseMipLevel   = 0;
	//		subresource_range.levelCount     = para_pack.default_image_CI.mipLevels;
	//		subresource_range.baseArrayLayer = 0;
	//		subresource_range.layerCount     = para_pack.default_image_CI.arrayLayers;
	//
	//		result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_layout_right_aft_creation, DeviceManager::CommandPoolType::transfor_command_pool, subresource_range);
	//	}
	//	else
	//	{
	//		VkImageSubresourceRange subresource_range;
	//		subresource_range.baseMipLevel   = 0;
	//		subresource_range.levelCount     = para_pack.default_image_CI.mipLevels;
	//		subresource_range.baseArrayLayer = 0;
	//		subresource_range.layerCount     = para_pack.default_image_CI.arrayLayers;
	//		result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_layout_right_aft_creation, DeviceManager::CommandPoolType::graphics_command_pool, subresource_range);
	//	}
	//
	//	return;
	//}

}        // namespace Anni
