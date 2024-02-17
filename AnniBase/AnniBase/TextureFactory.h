#pragma once

#include "BufferFactory.h"
#include "GeneralPurposeImage.h"
#include "ImageFactory.h"
#include "ImgViewWrapper.h"
#include "SwapchainImage.h"
#include "VkTexture.h"
#include "AnniUtil.h"

#include "VirtualTexture.h"
#include <string>
#include <unordered_map>
#include <ktx.h>
#include <ktxvulkan.h>
#include <stb_image.h>



namespace Anni
{
	class VkTextureFactory
	{
	public:
		VkTextureFactory(
			GraphicsComponent& gfx_,
			ImageFactory& img_factory_,
			BufferFactory& buffer_factory_);

		VkTextureFactory() = delete;
		VkTextureFactory(const VkTextureFactory&) = delete;
		VkTextureFactory& operator=(const VkTextureFactory&) = delete;
		VkTextureFactory(VkTextureFactory&&) = delete;
		VkTextureFactory& operator=(VkTextureFactory&&) = delete;


		[[nodiscard]] std::shared_ptr<VkTexture> ProduceTextureFromImgPath(const std::string& image_path, vk::Format format_of_image_, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, const vk::ImageAspectFlags aspect_flag = vk::ImageAspectFlagBits::eColor);

		[[nodiscard]] std::shared_ptr<VkTexture> ProduceTextureFromBufferReFac(ImageCIEnhanced texture_img_CI, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, const void* const data) const;

		[[nodiscard]] std::shared_ptr<VkTexture> ProduceUnfilledTextureReFac(ImageCIEnhanced texture_img_PP, std::optional<vk::SamplerCreateInfo> sampler_CI, std::optional<vk::ImageViewCreateInfo> img_view_CI) const;

		[[nodiscard]] std::vector<std::shared_ptr<VkTexture>> ProduceUnfilledTextureArrayReFac(ImageCIEnhanced texture_img_PP, std::optional<vk::SamplerCreateInfo> sampler_CI, std::optional<vk::ImageViewCreateInfo> img_view_CI, size_t bundle_size) const;



		[[nodiscard]] std::vector<std::shared_ptr<VkTexture>> ProduceSwapTextureArrayReFac(ImageCIEnhanced texture_img_CI, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_) const;

		[[nodiscard]] std::shared_ptr<VkTexture> ProduceSwapTexture(ImageCIEnhanced texture_img_CI, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, size_t index) const;




		void ResetTexSampler(vk::SamplerCreateInfo sampler_CI_, VkTexture& result_tex) const;
		void ResetTexSampler(vk::SamplerCreateInfo sampler_CI_, VkTexture::TexturePtrBundle& tex_ptr_bundle) const;
		void ResetTexImgView(vk::ImageViewCreateInfo img_view_CI_, VkTexture& result_tex) const;

		void GenerateMipMaps();
	public:
		void ActualizeVirtualResource(RenderGraphV1::VirtualTexture& vtex);
	private:
		//帮助函数
		[[nodiscard]] std::shared_ptr<GeneralPurposeImageReFac> InitLdrImgFromFile(const std::string& image_path, const vk::Format format_of_image, const vk::ImageAspectFlags aspect_flag) const;
		[[nodiscard]] std::shared_ptr<GeneralPurposeImageReFac> InitHdrImgFromFile(const std::string& image_path, const vk::Format format_of_image_, const vk::ImageAspectFlags aspect_flag) const;
		[[nodiscard]] std::shared_ptr<GeneralPurposeImageReFac> InitKtxImgFromFileLdrHdr(const std::string& image_path, const vk::Format format_of_image, const vk::ImageAspectFlags aspect_flag);

		void ProcessViewAndSamplerReFac(std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, VkTexture& result_tex, ImageBaseReFac& img) const;


	private:
		//帮助函数
		//ONLY SUPPORT 1 layer 1 level 1 depth!For multiple layer and level, go to load ktx or other file formats!
		template <typename ColorChannel>
		[[nodiscard]] std::shared_ptr<GeneralPurposeImageReFac> Init1Mip1LayerImgFromHostBufferReFac(ImageCIEnhanced& tex_img_CI, const void* const data) const;

	private:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;
		SwapchainManager& swapchain_manager;
		WindowsSys& window;
		ImageFactory& img_factory;
		BufferFactory& buffer_factory;
	};

	//帮助函数
	//ONLY SUPPORT 1 layer 1 level 1 depth!For multiple layer and level, go to load ktx or other file formats!

	template <typename ColorChannel>
	inline std::shared_ptr<GeneralPurposeImageReFac> VkTextureFactory::Init1Mip1LayerImgFromHostBufferReFac(ImageCIEnhanced& tex_img_CI, const void* const data) const
	{
		
		const vk::DeviceSize                      upload_size = tex_img_CI.vk_image_CI.extent.height * tex_img_CI.vk_image_CI.extent.width * 4 * sizeof(ColorChannel);
		std::shared_ptr<GeneralPurposeImageReFac> texture_image = img_factory.ProduceImage(tex_img_CI);
		const auto staging_buffer = buffer_factory.ProduceBufferReFac(upload_size, CI::StagingBuffer);

		//创建完staging buffer以后，把cpu中的texture中的数据拷贝进staging buffer
		staging_buffer->CopyFromHost((void const*)data, upload_size);

		//ONLY SUPPORT 1 layer 1 level 1 depth!For multiple layer and level, go to load ktx or other file formats!
		std::vector<vk::BufferImageCopy2> buffer_copy_regions;
		// Calculate offset into staging buffer for the current mip level
		constexpr vk::DeviceSize offset = 0;
		// Setup a buffer image copy structure for the current mip level
		vk::BufferImageCopy2 buffer_copy_region{};
		buffer_copy_region.bufferOffset = offset;

		buffer_copy_region.imageSubresource.aspectMask = tex_img_CI.aspect_flags;
		buffer_copy_region.imageSubresource.mipLevel = 0;
		buffer_copy_region.imageSubresource.baseArrayLayer = 0;
		buffer_copy_region.imageSubresource.layerCount = 1;

		buffer_copy_region.imageExtent.width = (tex_img_CI.vk_image_CI.extent.width);        
		buffer_copy_region.imageExtent.height = (tex_img_CI.vk_image_CI.extent.height);
		buffer_copy_region.imageExtent.depth = 1;

		buffer_copy_regions.push_back(buffer_copy_region);

		texture_image = img_factory.ProduceImage(tex_img_CI);
		texture_image->CopyBufferToImageOnTransferUpload(staging_buffer->GetGPUBuffer(), buffer_copy_regions);
		return texture_image;
	}

}        // namespace Anni



