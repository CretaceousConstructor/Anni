#pragma once
#include "GeneralPurposeImage.h"
#include "SwapchainImage.h"
#include "ImageCI.h"



namespace Anni
{
	class ImageFactory
	{
	public:
		ImageFactory(
			GraphicsComponent& gfx_,
			VmaAllocatorWrapper& vam_allocator_,
			VkTimelineSemPoolUnsafe& sem_pool_
			);
		~ImageFactory() = default;

		ImageFactory() = delete;
		ImageFactory(const ImageFactory&) = delete;
		ImageFactory& operator=(const ImageFactory&) = delete;

		ImageFactory(ImageFactory&&) = delete;
		ImageFactory& operator=(ImageFactory&&) = delete;

		[[nodiscard]] std::shared_ptr<GeneralPurposeImageReFac> ProduceImage(ImageCIEnhanced& img_CI) const;

	private:
		//[[nodiscard]] vk::Image        BuildImage(ImageCIEnhanced& img_CI) const;
		//[[nodiscard]] vk::DeviceMemory CreateAndBindMemory(ImageCIEnhanced& img_CI, vk::Image temp_image) const;

		//static void                  TransitionImageLayout(ImageCIEnhanced &img_CI, const std::shared_ptr<ImageBaseReFac> &result);

	protected:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		VkTimelineSemPoolUnsafe& sem_pool;
	protected:
		VmaAllocatorWrapper& vam_allocator;

	};
}        // namespace Anni
