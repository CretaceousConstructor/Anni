#pragma once

#include "GeneralPurposeImage.h"
#include "ImageCI.h"
#include "ImgViewWrapper.h"
#include "QueueManager.h"
#include "SamplerWrapper.h"
#include "RsrcUsage.h"
#include "Resource.h"
#include "SyncInfo.h"
#include <memory>
#include <string>

namespace Anni
{
	class VkTexture
	{
	public:
		friend class VkTextureFactory;
	public:
		using TexturePtrBundle = std::vector<std::shared_ptr<VkTexture>>;
		using TexturePtr = std::shared_ptr<VkTexture>;

	public:
		struct Descriptor
		{
			ImageCIEnhanced                      tex_img_CI;
			std::optional<vk::ImageViewCreateInfo> img_view_CI;
			std::optional<vk::SamplerCreateInfo>   sampler_CI;
		};

		VkTexture(
			GraphicsComponent& gfx_, std::optional<std::string> image_path, std::shared_ptr<ImageBaseReFac> image_,
			ImgSyncInfo sync_info_onload_
		);

		VkTexture() = delete;
		~VkTexture() = default;

		VkTexture(const VkTexture&) = delete;
		VkTexture& operator=(const VkTexture&) = delete;

		VkTexture(VkTexture&&) = delete;
		VkTexture& operator=(VkTexture&&) = delete;

	public:


		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(RenderGraphV1::TexUsage& tex_usage, VkDescriptorSet set);

		//[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0);
		//[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0);


		void ResetImgView(std::shared_ptr<ImgViewWrapper> tex_image_view_)
		{
			tex_image_view = tex_image_view_;
		}
		void ResetSampler(std::shared_ptr<SamplerWrapper> tex_sampler_)
		{
			tex_sampler = tex_sampler_;
		}

		ImgSyncInfo GetSynInfoOnLoad()
		{
			return  sync_info_onload;
		}


		std::shared_ptr<ImgViewWrapper>& GetTextureImageViewPtr()
		{
			return tex_image_view;
		}

		std::shared_ptr<SamplerWrapper>& GetTextureSamplerPtr()
		{
			return tex_sampler;
		}




	public:
		[[nodiscard]] std::shared_ptr<ImageBaseReFac> GetTextureImage();
		[[nodiscard]] VkImage                         GetTextureRawImage() const;

		[[nodiscard]] ImgViewWrapper& GetTextureImageView() const;
		[[nodiscard]] SamplerWrapper& GetTextureSampler() const;

		[[nodiscard]] std::string GetTextureName() const;



		[[nodiscard]] const uint32_t& GetSwapImageIndex() const
		{
			VULKAN_HPP_ASSERT(swap_image_index, "This is not a swap image");
			return swap_image_index.value();
		}


	public:
		//void InsertImageMemoryBarrier(VkCommandBuffer cmd_buffer, const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier);
		////这个函数目前需要修改，修改的目的是让用户去解决具体该怎么写image barrier，而不是通过一堆if else去假设。
		//void InsertImageMemoryBarrier(const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier);

		//The following syn primitives are used for swap image
		std::shared_ptr <BinarySemWrapper> present_finished_sem;
		std::shared_ptr <BinarySemWrapper> ready_for_present_sem;
		//****************************************************************
		std::shared_ptr <TimelineSemWrapper> present_finished_helper_sem;
		std::shared_ptr <TimelineSemWrapper> swap_img_rdy_4_rendering_helper_sem;
		//****************************************************************



	private:
		std::optional<VkSamplerCreateInfo>   sampler_CI;
		std::optional<VkImageViewCreateInfo> img_view_CI;

	private:
		void SetCISamplerAndImgView(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_);


	private:
		std::optional<std::string>      tex_name;
		std::shared_ptr<ImageBaseReFac> tex_image;

		std::shared_ptr<ImgViewWrapper> tex_image_view;
		std::shared_ptr<SamplerWrapper> tex_sampler;


	private:
		//VkBool32 ktx_use_staging = true;
		ImgSyncInfo sync_info_onload;

	private:
		void SetImgViewCI(vk::ImageViewCreateInfo image_view_);
		void SetSamplerCI(vk::SamplerCreateInfo texture_sampler_);
	private:
		std::optional<uint32_t> swap_image_index;
	public:
		bool IsSwapTexture()
		{
			return swap_image_index.has_value();
		}

	private:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;
	};

}        // namespace Anni
