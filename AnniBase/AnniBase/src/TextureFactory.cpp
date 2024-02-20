#include "TextureFactory.h"



namespace Anni
{
	VkTextureFactory::VkTextureFactory(
		GraphicsComponent& gfx_,
		ImageFactory& img_factory_,
		BufferFactory& buffer_factory_

	) :
		gfx(gfx_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), window(gfx.Window()), img_factory(img_factory_), buffer_factory(buffer_factory_)
	{
	}

	//Checked
	std::shared_ptr<VkTexture> VkTextureFactory::ProduceTextureFromImgPath(const std::string& image_path, vk::Format format_of_image_, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, const vk::ImageAspectFlags aspect_flag)
	{
		std::shared_ptr<GeneralPurposeImageReFac> tex_img;
		const auto                                file_extension = Util::GetFileExtensionName(image_path);

		// ktx.h can process both hdr and ldr imges.
		if (std::string("ktx") == file_extension)
		{
			tex_img = InitKtxImgFromFileLdrHdr(image_path, format_of_image_, aspect_flag);

			if (img_view_CI_)
			{
				img_view_CI_->subresourceRange.levelCount = tex_img->GetCI().vk_image_CI.mipLevels;
				img_view_CI_->subresourceRange.layerCount = tex_img->GetCI().vk_image_CI.arrayLayers;
			}

			if (sampler_CI_)
			{
				sampler_CI_->maxLod = static_cast<float>(tex_img->GetCI().vk_image_CI.mipLevels);
			}
		}

		//TODO: Non-ktx texture mipmap generation


		//HDR images
		else if (stbi_is_hdr(image_path.c_str()))
		{
			tex_img = InitHdrImgFromFile(image_path, format_of_image_, aspect_flag);
		}
		//LDR images
		else
		{
			tex_img = InitLdrImgFromFile(image_path, format_of_image_, aspect_flag);
		}

		constexpr Anni::ImgSyncInfo syc_info_onload
		{
			.access_mask = vk::AccessFlagBits2::eTransferWrite,
			.stage_mask = vk::PipelineStageFlagBits2::eCopy,
			.layout_inpass = vk::ImageLayout::eTransferDstOptimal
		};

		auto result = std::make_shared<VkTexture>(gfx, image_path, tex_img, syc_info_onload);

		ProcessViewAndSamplerReFac(sampler_CI_, img_view_CI_, *result, *tex_img);

		return result;
	}

	//Checked
	std::shared_ptr<VkTexture> VkTextureFactory::ProduceTextureFromBufferReFac(ImageCIEnhanced texture_img_CI, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, const void* const data) const
	{
		std::shared_ptr<GeneralPurposeImageReFac> tex_img = Init1Mip1LayerImgFromHostBufferReFac<uint8_t>(texture_img_CI, data);

		constexpr Anni::ImgSyncInfo syc_info_onload{
			.access_mask = vk::AccessFlagBits2::eTransferWrite,
			.stage_mask = vk::PipelineStageFlagBits2::eCopy,
			.layout_inpass = vk::ImageLayout::eTransferDstOptimal };

		auto result = std::make_shared<VkTexture>(gfx, std::nullopt, tex_img, syc_info_onload);
		ProcessViewAndSamplerReFac(sampler_CI_, img_view_CI_, *result, *tex_img);

		return result;
	}

	//Checked

	std::shared_ptr<VkTexture> VkTextureFactory::ProduceUnfilledTextureReFac(ImageCIEnhanced texture_img_PP, std::optional<vk::SamplerCreateInfo> sampler_CI, std::optional<vk::ImageViewCreateInfo> img_view_CI) const
	{
		auto tex_img = img_factory.ProduceImage(texture_img_PP);

		ImgSyncInfo syc_info_onload{
			.access_mask = vk::AccessFlagBits2::eNone,
			.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
			.layout_inpass = vk::ImageLayout::eUndefined
		};

		auto result = std::make_shared<VkTexture>(gfx, std::nullopt, tex_img, syc_info_onload);

		ProcessViewAndSamplerReFac(sampler_CI, img_view_CI, *result, *tex_img);
		return result;
	}

	//Checked
	std::vector<std::shared_ptr<VkTexture>> VkTextureFactory::ProduceUnfilledTextureArrayReFac(ImageCIEnhanced texture_img_PP, std::optional<vk::SamplerCreateInfo> sampler_CI, std::optional<vk::ImageViewCreateInfo> img_view_CI, size_t bundle_size) const
	{
		std::vector<std::shared_ptr<VkTexture>> result;

		for (uint32_t i = 0; i < bundle_size; i++)
		{
			result.push_back(ProduceUnfilledTextureReFac(texture_img_PP, sampler_CI, img_view_CI));
		}

		return result;
	}

	void VkTextureFactory::ProcessViewAndSamplerReFac(std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, VkTexture& result_tex, ImageBaseReFac& img) const
	{
		if (img_view_CI_)
		{
			img_view_CI_->image = img.GetImage();
			result_tex.tex_image_view = std::make_shared<ImgViewWrapper>(device_manager, img_view_CI_.value());
		}

		if (sampler_CI_)
		{
			result_tex.tex_sampler = std::make_shared<SamplerWrapper>(device_manager, sampler_CI_.value());
		}

		result_tex.SetCISamplerAndImgView(sampler_CI_, img_view_CI_);
	}


	//Checked
	std::vector<std::shared_ptr<VkTexture>> VkTextureFactory::ProduceSwapTextureArrayReFac(ImageCIEnhanced texture_img_CI, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_) const
	{
		std::vector<std::shared_ptr<VkTexture>> result;
		const std::vector<vk::Image>& swap_images = swapchain_manager.GetSwapChainImages();

		for (uint32_t i = 0; i < swap_images.size(); i++)
		{
			texture_img_CI.vk_image_CI.format = swapchain_manager.GetSwapChainImageFormat();
			texture_img_CI.vk_image_CI.extent = swapchain_manager.GetSwapChainImageExtent();

			ImgSyncInfo syc_info_onload{
				.access_mask = vk::AccessFlagBits2::eNone,
				.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
				.layout_inpass = vk::ImageLayout::eUndefined
			};

			auto       tex_img = std::make_shared<SwapchainImageReFac>(gfx, swap_images[i], texture_img_CI);
			const auto tex = std::make_shared<VkTexture>(gfx, std::nullopt, tex_img, syc_info_onload);
			ProcessViewAndSamplerReFac(sampler_CI_, img_view_CI_, *tex, *tex_img);

			tex->swap_image_index = i;

			result.push_back(tex);
		}

		return result;
	}


	std::shared_ptr<VkTexture> VkTextureFactory::ProduceSwapTexture(ImageCIEnhanced texture_img_CI, std::optional<vk::SamplerCreateInfo> sampler_CI_, std::optional<vk::ImageViewCreateInfo> img_view_CI_, size_t index) const
	{
		std::shared_ptr<VkTexture> result;
		const std::vector<vk::Image>& swap_images = swapchain_manager.GetSwapChainImages();

		texture_img_CI.vk_image_CI.format = swapchain_manager.GetSwapChainImageFormat();
		texture_img_CI.vk_image_CI.extent = swapchain_manager.GetSwapChainImageExtent();



		ImgSyncInfo syc_info_onload{
			.access_mask = vk::AccessFlagBits2::eNone,
			.stage_mask = vk::PipelineStageFlagBits2::eTopOfPipe,
			.layout_inpass = vk::ImageLayout::eUndefined
		};

		auto       tex_img = std::make_shared<SwapchainImageReFac>(gfx, swap_images[index], texture_img_CI);
		const auto tex = std::make_shared<VkTexture>(gfx, std::nullopt, tex_img, syc_info_onload);
		ProcessViewAndSamplerReFac(sampler_CI_, img_view_CI_, *tex, *tex_img);

		return result;
	}



	void VkTextureFactory::ResetTexSampler(vk::SamplerCreateInfo sampler_CI_, VkTexture& result_tex) const
	{
		result_tex.tex_sampler = std::make_shared<SamplerWrapper>(device_manager, sampler_CI_);
		result_tex.SetSamplerCI(sampler_CI_);
		return;
	}

	void VkTextureFactory::ResetTexSampler(vk::SamplerCreateInfo sampler_CI_, VkTexture::TexturePtrBundle& tex_ptr_bundle) const
	{
		for (auto& tex : tex_ptr_bundle)
		{
			ResetTexSampler(sampler_CI_, *tex);
		}
	}

	void VkTextureFactory::ResetTexImgView(vk::ImageViewCreateInfo img_view_CI_, VkTexture& result_tex) const
	{
		img_view_CI_.image = result_tex.GetTextureRawImage();
		result_tex.tex_image_view = std::make_shared<ImgViewWrapper>(device_manager, img_view_CI_);
		result_tex.SetImgViewCI(img_view_CI_);
		return;
	}



	//TODO:
	inline void VkTextureFactory::GenerateMipMaps()
	{
		ASSERT_WITH_MSG(false, "Not implemented!");
	}

	void VkTextureFactory::ActualizeVirtualResource(RenderGraphV1::VirtualTexture& vtex)
	{
		ASSERT_WITH_MSG(vtex.descriptor.has_value(), "No discriptor of the given resource is provided.");

		const auto& dis = vtex.descriptor.value();
		if (!dis.tex_img_CI.image_path)
		{
			vtex.p_rsrc = ProduceUnfilledTextureReFac(dis.tex_img_CI, dis.sampler_CI, dis.img_view_CI);
		}
		else
		{
			vtex.p_rsrc = ProduceTextureFromImgPath(dis.tex_img_CI.image_path.value(), dis.tex_img_CI.vk_image_CI.format, dis.sampler_CI, dis.img_view_CI, dis.tex_img_CI.aspect_flags);
		}
	}

	std::shared_ptr<GeneralPurposeImageReFac> VkTextureFactory::InitLdrImgFromFile(const std::string& image_path, const vk::Format format_of_image_, const vk::ImageAspectFlags aspect_flags) const
	{
		//TODO: mipmap info(cubemap) and layee info(msaa)

		// Get the image data from the stbi loader
		int            width, height, nrChannels;
		unsigned char* data = stbi_load(image_path.c_str(), &width, &height, &nrChannels, 0);
		if (nullptr == data)
		{
			throw std::runtime_error("failed to load image with stb!");
		}

		unsigned char* buffer = nullptr;
		bool           delete_buffer = false;
		// If it's an RGB image (3 components), we convert it to RGBA as most Vulkan implementations don't support 8-Bit RGB-only formats
		if (nrChannels == 3)
		{
			buffer = new unsigned char[width * height * 4];
			unsigned char* rgba = buffer;
			const unsigned char* rgb = data;
			for (size_t i = 0; i < width * height; ++i)
			{
				memcpy(rgba, rgb, sizeof(unsigned char) * 3);
				rgba[3] = 0xff;
				rgba += 4;
				rgb += 3;
			}
			delete_buffer = true;
		}
		else        //4 components, ok
		{
			buffer = data;
		}

		assert(width > 0 && height > 0);

		//const ImageCIReFac tex_img_CI{image_path, format_of_image_, VkExtent3D(width, height, 1)};

		ImageCIEnhanced tex_img_CI = CI::GetTextureImgCI(format_of_image_, vk::Extent3D(width, height, 1), vk::ImageCreateFlags(VK_ZERO_FLAG), image_path);
		tex_img_CI.aspect_flags = aspect_flags;
		auto image = Init1Mip1LayerImgFromHostBufferReFac<unsigned char>(tex_img_CI, buffer);

		if (delete_buffer)
		{
			delete[] buffer;
		}
		stbi_image_free(data);
		return image;
	}

	std::shared_ptr<GeneralPurposeImageReFac> VkTextureFactory::InitHdrImgFromFile(const std::string& image_path, const vk::Format format_of_image_, const vk::ImageAspectFlags aspect_flags) const
	{
		//stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;

		//Get the image data from the stbi loader
		float* data = stbi_loadf(image_path.c_str(), &width, &height, &nrComponents, 0);

		assert(width > 0 && height > 0);
		if (nullptr == data)
		{
			throw std::runtime_error("failed to load HDR image!");
		}

		float* cpu_buffer = nullptr;
		bool   delete_buffer = false;
		// If it's an RGB image (3 components), we convert it to RGBA as most Vulkan implementations don't support 8-Bit RGB-only formats
		if (nrComponents == 3)
		{
			cpu_buffer = new float[width * height * 4];
			float* rgba = cpu_buffer;
			const float* rgb = data;
			for (size_t i = 0; i < width * height; ++i)
			{
				memcpy(rgba, rgb, sizeof(float) * 3);
				rgba[3] = 1.f;
				rgba += 4;
				rgb += 3;
			}
			delete_buffer = true;
		}
		else
		{
			cpu_buffer = data;
		}

		ImageCIEnhanced tex_img_CI = CI::GetTextureImgCI(format_of_image_, VkExtent3D(width, height, 1), vk::ImageCreateFlags(VK_ZERO_FLAG), image_path);
		tex_img_CI.aspect_flags = aspect_flags;
		auto image = Init1Mip1LayerImgFromHostBufferReFac<float>(tex_img_CI, cpu_buffer);

		if (delete_buffer)
		{
			delete[] cpu_buffer;
		}
		stbi_image_free(data);
		return image;
	}

	//This function should be move to other class
	std::shared_ptr<GeneralPurposeImageReFac> VkTextureFactory::InitKtxImgFromFileLdrHdr(const std::string& image_path, const vk::Format format_of_image, vk::ImageAspectFlags aspect_flag)
	{
		std::shared_ptr<GeneralPurposeImageReFac> texture_image;
		//const auto                                tex_name = image_path.substr(image_path.find_last_of("\\/") + 1, image_path.length());
		//========================================================================================================================
		ktxTexture* ktxTexture;
		//ktxTexture struct has a ktx_uint32_t glFormat field representing the image's format

		const ktxResult result = ktxTexture_CreateFromNamedFile(image_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);

		assert(result == KTX_SUCCESS);

		const ktx_uint8_t* const ktxTextureData = ktxTexture_GetData(ktxTexture);
		//const ktx_size_t         ktxTextureSize = ktxTexture_GetSize(ktxTexture);

		const ktx_size_t         ktxTextureSize = ktxTexture_GetDataSize(ktxTexture);
		//========================================================================================================================

		const vk::Extent3D temp_image_extent(
			ktxTexture->baseWidth,
			ktxTexture->baseHeight,
			ktxTexture->baseDepth
		);

		constexpr bool force_linear_tiling = false;
		bool           ktx_use_staging = true;

		if (force_linear_tiling)
		{
			// Don't use linear tiling if format is not supported for (linear) shader sampling
			// Get device properties for the requested texture format
			vk::FormatProperties2 formatProperties = device_manager.GetPhysicalDevice().getFormatProperties(format_of_image);
			ktx_use_staging = !(formatProperties.formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage);

		}

		if (ktx_use_staging)
		{
			// Copy data to an optimal tiled image
			// This loads the texture data into a host local buffer that is copied to the optimal tiled image on the device

			// Create a host-visible staging buffer that contains the raw image data
			// This buffer will be the data source for copying texture data to the optimal tiled image on the device

			//========================================================================================================================

			auto staging_buffer = buffer_factory.ProduceBuffer(ktxTextureSize, CI::StagingBuffer);

			//创建完staging buffer以后，把cpu-side中的texture中的数据拷贝进staging buffer
			//TODO: whether to use memReqs.size or just ktxTexureSize. 用ktxTexturesSize是对的。

			staging_buffer->CopyFromHost((void const*)ktxTextureData, ktxTextureSize);
			//========================================================================================================================

			//TODO:这里没有考虑3维材质的问题,目前只能处理 n layer，n mip，1 depth

			//Setup buffer copy regions for each mip level
			std::vector<vk::BufferImageCopy2> buffer_copy_regions;

			//const void*                 pNext;
			//VkDeviceSize                bufferOffset;
			//uint32_t                    bufferRowLength;
			//uint32_t                    bufferImageHeight;
			//VkImageSubresourceLayers    imageSubresource;
			//VkOffset3D                  imageOffset;
			//VkExtent3D                  imageExtent;

			for (uint32_t face = 0; face < ktxTexture->numFaces; face++)
			{
				for (uint32_t level = 0; level < ktxTexture->numLevels; level++)
				{
					// Calculate offset into staging buffer for the current mip level
					ktx_size_t     offset;
					KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
					assert(ret == KTX_SUCCESS);
					// Setup a buffer image copy structure for the current mip level
					vk::BufferImageCopy2 bufferCopyRegion{};

					bufferCopyRegion.bufferOffset = offset;

					bufferCopyRegion.imageSubresource.aspectMask = aspect_flag;
					bufferCopyRegion.imageSubresource.mipLevel = level;
					bufferCopyRegion.imageSubresource.baseArrayLayer = face;
					bufferCopyRegion.imageSubresource.layerCount = 1;

					bufferCopyRegion.imageExtent.width = (ktxTexture->baseWidth) >> level;        //除法
					bufferCopyRegion.imageExtent.height = (ktxTexture->baseHeight) >> level;
					bufferCopyRegion.imageExtent.depth = 1;

					buffer_copy_regions.push_back(bufferCopyRegion);
				}
			}

			vk::ImageCreateFlags tex_img_CI_flags{ VK_ZERO_FLAG };

			if (ktxTexture->numFaces > 1)
			{
				tex_img_CI_flags |= vk::ImageCreateFlagBits::eCubeCompatible;
			}

			ImageCIEnhanced tex_img_CI = CI::GetTextureImgCI(format_of_image, temp_image_extent, tex_img_CI_flags, image_path);

			tex_img_CI.vk_image_CI.mipLevels = ktxTexture->numLevels;
			tex_img_CI.vk_image_CI.arrayLayers = ktxTexture->numFaces;

			texture_image = img_factory.ProduceImage(tex_img_CI);
			texture_image->CopyBufferToImageOnTransferUpload(staging_buffer->GetGPUBuffer(), buffer_copy_regions);

		}
		else
		{
			//TODO: handle situation when we can't use staging buffer
		}


		ktxTexture_Destroy(ktxTexture);
		//========================================================================================================================

		return texture_image;
	}

}        // namespace Anni
