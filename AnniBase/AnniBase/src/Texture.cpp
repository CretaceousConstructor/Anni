#include "Texture.h"

namespace Anni
{
	Texture::Texture(
		GraphicsComponent& gfx_, std::optional<std::string> image_path, std::shared_ptr<ImageBaseReFac> image_,
		ImgSyncInfo sync_info_onload_
	) :

		tex_name(std::move(image_path)),
		tex_image(std::move(image_)),
		sync_info_onload(sync_info_onload_),
		gfx(gfx_),
		device_manager(gfx_.DeviceMan())
	{
	}

	void Texture::SetImgViewCI(vk::ImageViewCreateInfo image_view_CI_)
	{
		img_view_CI = image_view_CI_;
	}

	void Texture::SetSamplerCI(vk::SamplerCreateInfo texture_sampler_CI)
	{
		sampler_CI = texture_sampler_CI;
	}



	vk::WriteDescriptorSet Texture::GetWriteDescriptorSetInfo(RenderGraphV1::TexUsage& tex_usage, vk::DescriptorSet set)
	{
		if (!tex_usage.img_view_CI)
		{
			tex_usage.image_info.imageView = GetTextureImageView().GetRawImgView();
		}
		else
		{
			tex_usage.img_view = std::make_shared<ImgViewWrapper>(device_manager, tex_usage.img_view_CI.value());
			tex_usage.image_info.imageView = tex_usage.img_view->GetRawImgView();
		}

		if (!tex_usage.sampler_CI)
		{
			tex_usage.image_info.sampler = GetTextureSampler().GetRawSampler();
		}
		else
		{
			tex_usage.sampler = std::make_shared<SamplerWrapper>(device_manager, tex_usage.sampler_CI.value());
			tex_usage.image_info.sampler = tex_usage.sampler->GetRawSampler();
		}
		tex_usage.image_info.imageLayout = tex_usage.sync_info.layout_inpass;

		vk::WriteDescriptorSet temp_write_descriptor_set{};
		temp_write_descriptor_set.dstSet = set;
		temp_write_descriptor_set.dstBinding = tex_usage.desc_info.slot_info.binding;
		temp_write_descriptor_set.dstArrayElement = tex_usage.desc_info.slot_info.array_element;
		temp_write_descriptor_set.descriptorType = tex_usage.desc_info.descriptor_type;

		temp_write_descriptor_set.setImageInfo(tex_usage.image_info);
		return temp_write_descriptor_set;
	}

	void Texture::ResetImgView(std::shared_ptr<ImgViewWrapper>& tex_image_view_)
	{
		tex_image_view = tex_image_view_;
	}

	void Texture::ResetSampler(std::shared_ptr<SamplerWrapper>& tex_sampler_)
	{
		tex_sampler = tex_sampler_;
	}


	vk::WriteDescriptorSet Texture::GetWriteDescriptorSetInfoForModel(const RenderGraphV1::TexUsage& tex_usage)
	{
		tex_img_info.imageView = GetTextureImageView().GetRawImgView();
		tex_img_info.sampler = GetTextureSampler().GetRawSampler();
		tex_img_info.imageLayout = tex_usage.sync_info.layout_inpass;

		vk::WriteDescriptorSet temp_write_descriptor_set{};
		temp_write_descriptor_set.setImageInfo(tex_img_info);
		return temp_write_descriptor_set;
	}

	ImgSyncInfo& Texture::GetSynInfoOnLoad()
	{
		return  sync_info_onload;
	}



	std::shared_ptr<ImgViewWrapper>& Texture::GetTextureImageViewPtr()
	{
		return tex_image_view;
	}


	std::shared_ptr<SamplerWrapper>& Texture::GetTextureSamplerPtr()
	{
		return tex_sampler;
	}





	std::shared_ptr<ImageBaseReFac> Texture::GetTextureImage()
	{
		return tex_image;
	}

	VkImage Texture::GetTextureRawImage() const
	{
		return tex_image->GetImage();
	}

	ImgViewWrapper& Texture::GetTextureImageView() const
	{
		return *tex_image_view;
	}

	SamplerWrapper& Texture::GetTextureSampler() const
	{
		return *tex_sampler;
	}

	[[nodiscard]] std::string Texture::GetTextureName() const
	{
		assert(tex_name);
		return tex_name.value();
	}

	void Texture::SetCISamplerAndImgView(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_)
	{
		sampler_CI = sampler_CI_;
		img_view_CI = img_view_CI_;
	}

	[[nodiscard]] const uint32_t& Texture::GetSwapImageIndex() const
	{
		ASSERT_WITH_MSG(swap_image_index, "This is not a swap image");
		return swap_image_index.value();
	}

	bool Texture::IsSwapTexture()
	{
		return swap_image_index.has_value();
	}


}        // namespace Anni
