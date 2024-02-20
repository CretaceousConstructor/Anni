#pragma once

#include "ImgViewWrapper.h"
#include "SamplerWrapper.h"
#include "AttachmentInfo.h"
#include "TimelineSemWrapper.h"
#include "VkSemaphoreBundle.h"
#include "Resource.h"
#include "SyncInfo.h"



namespace Anni::RenderGraphV1
{

	// Base usage class
	class IRsrcUsage
	{
	public:
		enum class RsrcOrigin
		{
			Unknown,
			FromOutSide,
			FromSourcePass,
			FromPreviousFrame,
			EstablishedInSitu
		};


		IRsrcUsage(
			RsrcType rsrc_t_, RsrcAccessTypeRG access_t_) :
			rsrc_type(rsrc_t_), access_type(access_t_)
		{
		}


		IRsrcUsage(RsrcType rsrc_t_, RsrcAccessTypeRG access_t_, RsrcOrigin origin_,
			std::shared_ptr<BinarySemWrapper>   available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper>   finished_using_semaphore_ = nullptr
		)
			:rsrc_type(rsrc_t_), access_type(access_t_), origin(origin_)
		{
		}

		virtual ~IRsrcUsage() = 0;

		[[nodiscard]] RsrcType GetRsrcType() const
		{
			return rsrc_type;
		}

		[[nodiscard]] RsrcAccessTypeRG GetRsrcAccessType() const
		{
			return access_type;
		}
		[[nodiscard]] RsrcOrigin GetRsrcOrigin() const
		{
			return origin;
		}
		void SetRsrcOrigin(IRsrcUsage::RsrcOrigin origin_)
		{
			origin = origin_;
		}


		RsrcType rsrc_type{};
		RsrcAccessTypeRG access_type{};
		RsrcOrigin origin{};
	};


	class TexUsage final : public IRsrcUsage
	{
	public:
		// use as model textures importing
		TexUsage(
			ImgSyncInfo      sync_info_
		) :
			IRsrcUsage(RsrcType::ModelTextures, RsrcAccessTypeRG::Read),
			sync_info(sync_info_)
		{

		}

		// use as texture
		TexUsage(
			vk::Format       format_,
			DescriptorInfo desc_info_,
			ImgSyncInfo    sync_info_,
			RsrcAccessTypeRG access_t,

			std::optional<vk::ImageViewCreateInfo>      img_view_CI_ = std::nullopt,
			std::optional<vk::SamplerCreateInfo>        sampler_CI_ = std::nullopt,

			std::shared_ptr<BinarySemWrapper>   available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper>   finished_using_semaphore_ = nullptr

		)
			:
			IRsrcUsage(RsrcType::Texture, access_t),
			format(format_),
			img_view_CI(img_view_CI_),
			sampler_CI(sampler_CI_),
			desc_info(desc_info_),
			sync_info(sync_info_)
		{

		}

		ImgSyncInfo& GetSynInfo()
		{
			return sync_info;
		}

		~TexUsage() = default;
		TexUsage() = delete;

		vk::Format format;
		std::shared_ptr<ImgViewWrapper>      img_view;
		std::shared_ptr<SamplerWrapper>      sampler;

		std::optional<vk::ImageViewCreateInfo>      img_view_CI;
		std::optional<vk::SamplerCreateInfo>      sampler_CI;

		DescriptorInfo desc_info;
		ImgSyncInfo sync_info;

		vk::DescriptorImageInfo image_info{};
	};

	class AttachUsage final : public IRsrcUsage
	{
	public:


		AttachUsage(
			vk::Format format_,
			AttachmentInfo attach_info_,
			ImgSyncInfo sync_info_,
			RsrcAccessTypeRG access_t_,

			std::optional<VkImageViewCreateInfo>      img_view_CI_ = std::nullopt,
			std::optional<VkSamplerCreateInfo>        sampler_CI_ = std::nullopt,

			std::shared_ptr<BinarySemWrapper>   available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper>   finished_using_semaphore_ = nullptr

		) :
			IRsrcUsage(RsrcType::Attachment, access_t_),
			format(format_),
			img_view_CI(img_view_CI_),
			sampler_CI(sampler_CI_),
			attach_info(attach_info_),
			sync_info(sync_info_)

		{

		}


		ImgSyncInfo& GetSynInfo()
		{
			return sync_info;
		}


		vk::RenderingAttachmentInfo GetVkRenderingAttachmentInfo()
		{
			vk::RenderingAttachmentInfo rendering_attachment_info;
			rendering_attachment_info.imageView = img_view->GetRawImgView();
			rendering_attachment_info.imageLayout = sync_info.layout_inpass;

			rendering_attachment_info.loadOp = attach_info.load_op;
			rendering_attachment_info.storeOp = attach_info.store_op;
			rendering_attachment_info.clearValue = attach_info.clear_value;
			rendering_attachment_info.resolveMode = attach_info.resolve_mode;

			if (resolve_img_view)
			{
				rendering_attachment_info.resolveImageView = resolve_img_view->GetRawImgView();
			}
			rendering_attachment_info.resolveImageLayout = resolve_target_layout;

			return rendering_attachment_info;
		}

		DynamicRenderingAttachmentFormatInfo GetDynamicRenderingAttachmentFormatInfo()
		{
			return DynamicRenderingAttachmentFormatInfo(attach_info.attach_type, format, attach_info.attachment_index);
		}

		AttachUsage() = delete;
		~AttachUsage() = default;


		void BindResolveTarget(std::shared_ptr<ImgViewWrapper> resolve_img_view_, vk::ImageLayout resolve_target_layout_)
		{
			resolve_img_view = std::move(resolve_img_view_);
			resolve_target_layout = resolve_target_layout_;
		}


	public:
		// common ones
		vk::Format format;
		std::shared_ptr<ImgViewWrapper>      img_view;
		std::shared_ptr<SamplerWrapper>      sampler;

		std::optional<vk::ImageViewCreateInfo>      img_view_CI;
		std::optional<vk::SamplerCreateInfo>        sampler_CI;

		AttachmentInfo attach_info;
		ImgSyncInfo    sync_info;
	private:
		std::shared_ptr<ImgViewWrapper> resolve_img_view;
		vk::ImageLayout resolve_target_layout{ vk::ImageLayout::eUndefined};

	};





	class BufUsage final : public IRsrcUsage
	{
	public:
		BufUsage(
			DescriptorInfo desc_info_,
			BufSyncInfo	sync_info_,
			RsrcAccessTypeRG access_t_,

			std::shared_ptr<BinarySemWrapper>   available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper>   finished_using_semaphore_ = nullptr
		)
			:
			IRsrcUsage(RsrcType::Buffer, access_t_),
			desc_info(desc_info_),
			sync_info(sync_info_)
		{
		}

		BufUsage(
			RsrcType rsrc_type_,
			BufSyncInfo	sync_info_,
			RsrcAccessTypeRG access_t_,

			std::shared_ptr<BinarySemWrapper>   available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper>   finished_using_semaphore_ = nullptr
		) :
			IRsrcUsage(rsrc_type_, access_t_),
			sync_info(sync_info_)
		{

		}

		BufSyncInfo& GetSynInfo()
		{
			return sync_info;
		}

		BufUsage() = delete;
		~BufUsage() = default;

		DescriptorInfo desc_info;
		BufSyncInfo    sync_info;
		vk::DescriptorBufferInfo des_buf_info;
	};



}

