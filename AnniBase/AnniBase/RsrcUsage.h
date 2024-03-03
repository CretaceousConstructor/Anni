#pragma once

#include "ImgViewWrapper.h"
#include "SamplerWrapper.h"
#include "AttachmentInfo.h"
#include "TimelineSemWrapper.h"
#include "BinarySemWrapper.h"
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
			RsrcType rsrc_t_, RsrcAccessTypeRG access_t_);


		IRsrcUsage(RsrcType rsrc_t_, RsrcAccessTypeRG access_t_, RsrcOrigin origin_,
		           std::shared_ptr<BinarySemWrapper> available_semaphore_ = nullptr,
		           std::shared_ptr<BinarySemWrapper> finished_using_semaphore_ = nullptr
		);

		virtual ~IRsrcUsage();

		[[nodiscard]] RsrcType GetRsrcType() const;
		[[nodiscard]] RsrcAccessTypeRG GetRsrcAccessType() const;
		[[nodiscard]] RsrcOrigin GetRsrcOrigin() const;
		void SetRsrcOrigin(RsrcOrigin origin_);


		RsrcType rsrc_type{};
		RsrcAccessTypeRG access_type{};
		RsrcOrigin origin{};
	};


	class TexUsage final : public IRsrcUsage
	{
	public:
		// use as model textures importing
		explicit TexUsage(
			const ImgSyncInfo& sync_info_
		);

		// use as texture
		TexUsage(
			vk::Format format_,
			const DescriptorInfo& desc_info_,
			const ImgSyncInfo& sync_info_,
			RsrcAccessTypeRG access_t,

			const std::optional<vk::ImageViewCreateInfo>& img_view_CI_ = std::nullopt,
			const std::optional<vk::SamplerCreateInfo>& sampler_CI_ = std::nullopt,

			std::shared_ptr<BinarySemWrapper> available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper> finished_using_semaphore_ = nullptr
		);

		ImgSyncInfo& GetSynInfo();
		~TexUsage() override = default;
		TexUsage() = delete;

		vk::Format format;
		std::shared_ptr<ImgViewWrapper> img_view;
		std::shared_ptr<SamplerWrapper> sampler;

		std::optional<vk::ImageViewCreateInfo> img_view_CI;
		std::optional<vk::SamplerCreateInfo> sampler_CI;

		DescriptorInfo desc_info;
		ImgSyncInfo sync_info;

		vk::DescriptorImageInfo image_info{};
	};

	class AttachUsage final : public IRsrcUsage
	{
	public:
		AttachUsage(
			vk::Format format_,
			const AttachmentInfo& attach_info_,
			const ImgSyncInfo& sync_info_,
			RsrcAccessTypeRG access_t_,

			const std::optional<VkImageViewCreateInfo>& img_view_CI_ = std::nullopt,
			const std::optional<VkSamplerCreateInfo>& sampler_CI_ = std::nullopt,

			std::shared_ptr<BinarySemWrapper> available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper> finished_using_semaphore_ = nullptr
		);


		ImgSyncInfo& GetSynInfo();


		vk::RenderingAttachmentInfo GetVkRenderingAttachmentInfo() const;

		DynamicRenderingAttachmentFormatInfo GetDynamicRenderingAttachmentFormatInfo() const;

		AttachUsage() = delete;
		~AttachUsage() override = default;

		void BindResolveTarget(std::shared_ptr<ImgViewWrapper> resolve_img_view_,
		                       vk::ImageLayout resolve_target_layout_);

	public:
		// common ones
		vk::Format format;
		std::shared_ptr<ImgViewWrapper> img_view;
		std::shared_ptr<SamplerWrapper> sampler;

		std::optional<vk::ImageViewCreateInfo> img_view_CI;
		std::optional<vk::SamplerCreateInfo> sampler_CI;

		AttachmentInfo attach_info;
		ImgSyncInfo sync_info;

	private:
		std::shared_ptr<ImgViewWrapper> resolve_img_view;
		vk::ImageLayout resolve_target_layout{vk::ImageLayout::eUndefined};
	};


	class BufUsage final : public IRsrcUsage
	{
	public:
		BufUsage(
			const DescriptorInfo& desc_info_,
			const BufSyncInfo& sync_info_,
			RsrcAccessTypeRG access_t_,

			std::shared_ptr<BinarySemWrapper> available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper> finished_using_semaphore_ = nullptr
		);

		BufUsage(
			RsrcType rsrc_type_,
			const BufSyncInfo& sync_info_,
			RsrcAccessTypeRG access_t_,

			std::shared_ptr<BinarySemWrapper> available_semaphore_ = nullptr,
			std::shared_ptr<BinarySemWrapper> finished_using_semaphore_ = nullptr
		);

		BufSyncInfo& GetSynInfo()
		{
			return sync_info;
		}

		BufUsage() = delete;
		~BufUsage() override = default;

		DescriptorInfo desc_info;
		BufSyncInfo sync_info;
		vk::DescriptorBufferInfo des_buf_info;
	};
}
