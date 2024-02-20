#pragma once
#include "AnniVkHeader.h"
#include "DeviceManager.h"
#include "SwapchainManager.h"

namespace Anni
{
	namespace CI
	{
		[[nodiscard]] vk::ImageViewCreateInfo PopulateSwapchainImgViewCI(const SwapchainManager& swapchain_manager);
		[[nodiscard]] vk::ImageViewCreateInfo PopulateDepthImgViewCI(const SwapchainManager& swapchain_manager);
		[[nodiscard]] vk::ImageViewCreateInfo PopulateDepthImgViewCI(const vk::Format depth_format);
		[[nodiscard]] vk::ImageViewCreateInfo PopulateTextureImgViewCI(vk::Format image_view_fomat_);
		[[nodiscard]] vk::ImageViewCreateInfo PopulateCubeMapImgViewCI(vk::Format image_view_fomat_);

	};        // namespace CI

	class ImgViewWrapper
	{
	public:
		ImgViewWrapper(DeviceManager& device_manager_, vk::ImageViewCreateInfo image_view_CI_);

		vk::ImageView GetRawImgView();
		~ImgViewWrapper() ;

		ImgViewWrapper() = delete;
		ImgViewWrapper(const ImgViewWrapper&) = delete;
		ImgViewWrapper& operator=(const ImgViewWrapper&) = delete;

		ImgViewWrapper(ImgViewWrapper&& rhs) = delete;
		ImgViewWrapper& operator=(ImgViewWrapper&&) = delete;

	private:
		DeviceManager&    device_manager;
		vk::ImageView     img_view;
		vk::ImageViewCreateInfo image_view_CI;
	};

}        // namespace Anni
