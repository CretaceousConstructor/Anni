#include "ImageCI.h"

namespace Anni
{
	ImageCIEnhanced CI::GetDepthImgCI(const vk::Format format_, const vk::Extent3D& image_extend_, vk::ImageCreateFlags flags)
	{
		ImageCIEnhanced result{};
		//*************************************
		result.vk_image_CI.format = format_;
		result.vk_image_CI.extent = image_extend_;
		//*************************************

		result.vk_image_CI.imageType = vk::ImageType::e2D;
		result.vk_image_CI.pNext = VK_NULL_HANDLE;
		result.vk_image_CI.flags = flags;
		result.vk_image_CI.mipLevels = 1;        //
		result.vk_image_CI.arrayLayers = 1;        //
		result.vk_image_CI.samples = vk::SampleCountFlagBits::e1;
		result.vk_image_CI.tiling = vk::ImageTiling::eOptimal;        //������Ϊ�ǲ���LINEAR�Ĵ洢��ʽ
		result.vk_image_CI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		result.vk_image_CI.sharingMode = vk::SharingMode::eExclusive;
		result.vk_image_CI.initialLayout = vk::ImageLayout::eUndefined;

		//*************************************
		result.mem_prop = vk::MemoryPropertyFlagBits::eDeviceLocal;
		result.aspect_flags = vk::ImageAspectFlagBits::eDepth;

		return result;
	}

	ImageCIEnhanced CI::GetDepthImgCI(SwapchainManager& swapchain_manager, vk::ImageCreateFlags flags)
	{
		ImageCIEnhanced result{};

		//*************************************
		result.vk_image_CI.format = swapchain_manager.FindDepthFormat();
		result.vk_image_CI.extent = swapchain_manager.GetSwapChainImageExtent();
		//*************************************


		result.vk_image_CI.imageType = vk::ImageType::e2D;
		result.vk_image_CI.pNext = VK_NULL_HANDLE;
		result.vk_image_CI.flags = flags;
		result.vk_image_CI.mipLevels = 1;        //
		result.vk_image_CI.arrayLayers = 1;        //
		result.vk_image_CI.samples = vk::SampleCountFlagBits::e1;
		result.vk_image_CI.tiling = vk::ImageTiling::eOptimal;        //��ΪLINEAR,TILING OPTIMAL���ִ洢��ʽ

		result.vk_image_CI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		result.vk_image_CI.sharingMode = vk::SharingMode::eExclusive;
		result.vk_image_CI.initialLayout = vk::ImageLayout::eUndefined;
		//*************************************
		result.mem_prop = vk::MemoryPropertyFlagBits::eDeviceLocal;
		result.aspect_flags = vk::ImageAspectFlagBits::eDepth;
	}

	ImageCIEnhanced CI::GetTextureImgCI(const vk::Format format_, const vk::Extent3D& image_extend_, vk::ImageCreateFlags flags, std::optional<std::string> image_path_)
	{
		ImageCIEnhanced result{};

		//*************************************
		result.vk_image_CI.format = format_;
		result.vk_image_CI.extent = image_extend_;
		//*************************************
		result.vk_image_CI.imageType = vk::ImageType::e2D;
		result.vk_image_CI.pNext = VK_NULL_HANDLE;
		result.vk_image_CI.flags = flags;

		result.vk_image_CI.mipLevels = 1;
		result.vk_image_CI.arrayLayers = 1;
		result.vk_image_CI.samples = vk::SampleCountFlagBits::e1;
		result.vk_image_CI.tiling = vk::ImageTiling::eOptimal;        //������Ϊ�ǲ���LINEAR�Ĵ洢��ʽ

		result.vk_image_CI.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		result.vk_image_CI.sharingMode = vk::SharingMode::eExclusive;

		//��specification�У���ʼlayoutֻ����undefined
		result.vk_image_CI.initialLayout = vk::ImageLayout::eUndefined;
		//*************************************
		result.mem_prop = vk::MemoryPropertyFlagBits::eDeviceLocal;
		result.aspect_flags = vk::ImageAspectFlagBits::eDepth;

		result.image_path = std::move(image_path_);
	}

	ImageCIEnhanced CI::GetSwapchainImgCI()
	{
		ImageCIEnhanced result{};
	}

}        // namespace Anni