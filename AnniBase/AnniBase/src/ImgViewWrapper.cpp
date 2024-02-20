#include "ImgViewWrapper.h"

namespace Anni
{
	vk::ImageViewCreateInfo CI::PopulateSwapchainImgViewCI(const SwapchainManager& swapchain_manager)
	{
		//typedef struct VkImageViewCreateInfo {
		//    VkStructureType            sType;
		//    const void*                pNext;
		//    VkImageViewCreateFlags     flags;
		//    VkImage                    image;
		//    VkImageViewType            viewType;
		//    VkFormat                   format;
		//    VkComponentMapping         components;
		//    VkImageSubresourceRange    subresourceRange;
		//} VkImageViewCreateInfo;

		vk::ImageViewCreateInfo default_image_view_CI{};
		default_image_view_CI.viewType = vk::ImageViewType::e2D;
		default_image_view_CI.format = swapchain_manager.GetSwapChainImageViewFormat(); //same as image format

		//default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		default_image_view_CI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

		default_image_view_CI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;        // specifies the color aspect.
		default_image_view_CI.subresourceRange.baseMipLevel = 0;
		default_image_view_CI.subresourceRange.levelCount = 1;
		default_image_view_CI.subresourceRange.baseArrayLayer = 0;
		default_image_view_CI.subresourceRange.layerCount = 1;

		return default_image_view_CI;
	}

	vk::ImageViewCreateInfo CI::PopulateDepthImgViewCI(const SwapchainManager& swapchain_manager)
	{
		vk::ImageViewCreateInfo default_image_view_CI{};
		default_image_view_CI.viewType = vk::ImageViewType::e2D;
		default_image_view_CI.format = swapchain_manager.FindDepthFormat();        //

		//default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		default_image_view_CI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

		default_image_view_CI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		default_image_view_CI.subresourceRange.baseMipLevel = 0;
		default_image_view_CI.subresourceRange.levelCount = 1;
		default_image_view_CI.subresourceRange.baseArrayLayer = 0;
		default_image_view_CI.subresourceRange.layerCount = 1;

		return default_image_view_CI;
	}

	vk::ImageViewCreateInfo CI::PopulateDepthImgViewCI(const vk::Format depth_format)
	{
		vk::ImageViewCreateInfo default_image_view_CI{};
		default_image_view_CI.viewType = vk::ImageViewType::e2D;
		default_image_view_CI.format = depth_format;        //

		//default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		//default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		default_image_view_CI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

		default_image_view_CI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		default_image_view_CI.subresourceRange.baseMipLevel = 0;
		default_image_view_CI.subresourceRange.levelCount = 1;
		default_image_view_CI.subresourceRange.baseArrayLayer = 0;
		default_image_view_CI.subresourceRange.layerCount = 1;

		return default_image_view_CI;
	}

	vk::ImageViewCreateInfo CI::PopulateTextureImgViewCI(vk::Format image_view_fomat_)
	{
		vk::ImageViewCreateInfo default_image_view_CI{};

		default_image_view_CI.viewType = vk::ImageViewType::e2D;
		default_image_view_CI.format = image_view_fomat_;

		default_image_view_CI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

		default_image_view_CI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		default_image_view_CI.subresourceRange.baseMipLevel = 0;
		default_image_view_CI.subresourceRange.levelCount = 1;
		default_image_view_CI.subresourceRange.baseArrayLayer = 0;
		default_image_view_CI.subresourceRange.layerCount = 1;

		return default_image_view_CI;
	}

	vk::ImageViewCreateInfo CI::PopulateCubeMapImgViewCI(vk::Format image_view_fomat_)
	{
		vk::ImageViewCreateInfo default_image_view_CI{};
		default_image_view_CI.viewType = vk::ImageViewType::eCube;
		default_image_view_CI.format = image_view_fomat_;

		default_image_view_CI.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

		default_image_view_CI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		default_image_view_CI.subresourceRange.baseMipLevel = 0;
		default_image_view_CI.subresourceRange.levelCount = 1;
		default_image_view_CI.subresourceRange.baseArrayLayer = 0;
		default_image_view_CI.subresourceRange.layerCount = 6;

		return default_image_view_CI;
	}



	ImgViewWrapper::ImgViewWrapper(DeviceManager& device_manager_, vk::ImageViewCreateInfo image_view_CI_) :
		device_manager(device_manager_), image_view_CI(image_view_CI_)
	{
		img_view = device_manager.GetLogicalDevice().createImageView(image_view_CI);
	}

	vk::ImageView ImgViewWrapper::GetRawImgView()
	{
		return img_view;
	}

	ImgViewWrapper::~ImgViewWrapper()
	{
		device_manager.GetLogicalDevice().destroyImageView(img_view);
	}


}        // namespace Anni
