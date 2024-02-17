#pragma once
#include "GeneralPurposeImage.h"
#include "SwapchainImage.h"
#include "ImageCI.h"



namespace Anni
{
	class ImageFactory
	{
	public:
		ImageFactory(GraphicsComponent& _gfx);
		~ImageFactory() = default;

		ImageFactory() = delete;
		ImageFactory(const ImageFactory&) = delete;
		ImageFactory& operator=(const ImageFactory&) = delete;

		ImageFactory(ImageFactory&&) = delete;
		ImageFactory& operator=(ImageFactory&&) = delete;

		[[nodiscard]] std::shared_ptr<GeneralPurposeImageReFac> ProduceImage(ImageCIEnhanced img_CI) const;

	private:
		[[nodiscard]] VkImage        BuildImage(ImageCIEnhanced& img_CI) const;
		[[nodiscard]] VkDeviceMemory CreateAndBindMemory(ImageCIEnhanced& img_CI, VkImage temp_image) const;

		//static void                  TransitionImageLayout(ImageCIEnhanced &img_CI, const std::shared_ptr<ImageBaseReFac> &result);

	protected:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;


	};
}        // namespace Anni
