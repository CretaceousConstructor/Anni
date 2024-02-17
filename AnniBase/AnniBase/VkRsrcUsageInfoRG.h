#pragma once
#include "VkLets.h"
#include "VkRsrcUsageInfo.h"

namespace Anni
{
	namespace RenderGraphV0
	{
		class PassNode;
	}

	namespace RenderGraphV0
	{
		class VkRsrcUsageInfoRGUniversalTex
		{
		public:
			VkRsrcUsageInfoRGUniversalTex(std::unique_ptr<VkUniversalTexUsageInfoRG> tex_usage_, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator tex_itr_);
			VkRsrcUsageInfoRGUniversalTex(std::unique_ptr<VkUniversalTexUsageInfoRG> tex_usage_, std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> model_tex_itrs_);



			VkRsrcUsageInfoRGUniversalTex() = default;
			~VkRsrcUsageInfoRGUniversalTex() = default;

			VkRsrcUsageInfoRGUniversalTex(VkRsrcUsageInfoRGUniversalTex&& other);
			VkRsrcUsageInfoRGUniversalTex& operator=(VkRsrcUsageInfoRGUniversalTex&& other) = delete;

			VkRsrcUsageInfoRGUniversalTex(const VkRsrcUsageInfoRGUniversalTex& other) = delete;
			VkRsrcUsageInfoRGUniversalTex& operator=(const VkRsrcUsageInfoRGUniversalTex& other) = delete;

			std::unique_ptr<VkUniversalTexUsageInfoRG>                            tex_usage;
			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator tex_itr;

			std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator> model_tex_itrs;


		};

		class VkRsrcUsageInfoRGBuf
		{
		public:
			VkRsrcUsageInfoRGBuf(std::unique_ptr<VkBufUsageInfoRG> buf_usage_, std::unordered_map<std::string, VirtualResource<Buffer>>::iterator buf_itr_);
			VkRsrcUsageInfoRGBuf() = delete;
			~VkRsrcUsageInfoRGBuf() = default;

			VkRsrcUsageInfoRGBuf(VkRsrcUsageInfoRGBuf&& other);
			VkRsrcUsageInfoRGBuf& operator=(VkRsrcUsageInfoRGBuf&& other) = delete;

			VkRsrcUsageInfoRGBuf(const VkRsrcUsageInfoRGBuf& other) = delete;
			VkRsrcUsageInfoRGBuf& operator=(const VkRsrcUsageInfoRGBuf& other) = delete;

			//VkBufUsageInfoRG &GetBufUsage() const;

			std::unique_ptr<VkBufUsageInfoRG>                                  buf_usage;
			std::unordered_map<std::string, VirtualResource<Buffer>>::iterator buffer;
		};















	}        // namespace RenderGraphV0

	namespace RenderGraphV0
	{


		void SetResolveTargetTexImgView(const VkRsrcUsageInfoRGUniversalTex& tex_usage_and_tex, const VkRsrcUsageInfoRGUniversalTex& resolve_target)
		{


			//const auto& usage_in_pass = *tex_usage_and_tex.tex_usage;
			//const auto& vsrc = (*tex_usage_and_tex.tex_itr).second;

			//resolve_tex = resolve_target.tex;
			//if (!info_in_pass.has_value())
			//{
			//	throw std::runtime_error("no valid usage info!");
			//}
			//else
			//{
			//	info_in_pass.value().resolveImageLayout = resolve_target.GetInfo().layout_inpass;
			//}
		}

		VkRenderingAttachmentInfo GetRenderingAttachmentInfo(VkRsrcUsageInfoRGUniversalTex& tex_usage_and_tex)
		{

			//Only valid for an Attachment
			assert(Vk::RsrcType::Attachment == tex_usage_and_tex.tex_usage->GetRsrcType());

			const auto& usage_in_pass = *tex_usage_and_tex.tex_usage;
			const auto& vsrc = (*tex_usage_and_tex.tex_itr).second;


			auto image_view_raw = vsrc.ptr_rsrc->GetTextureImageView().GetRawImgView();
			auto image_layout_raw = usage_in_pass.layout_inpass;

			VkRenderingAttachmentInfo  rendering_attachment_info;
			rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			rendering_attachment_info.imageView = image_view_raw;
			rendering_attachment_info.imageLayout = usage_in_pass.layout_inpass;
			rendering_attachment_info.loadOp = usage_in_pass.load_op;
			rendering_attachment_info.storeOp = usage_in_pass.store_op;
			rendering_attachment_info.clearValue = usage_in_pass.clear_value;
			rendering_attachment_info.resolveMode = usage_in_pass.resolve_mode;


			//TODO: resolve rex
			//if (resolve_tex)
			//{
			//	rendering_attachment_info.resolveImageView = resolve_tex->GetTextureImageView().GetRawImgView();
			//}

			//rendering_attachment_info.resolveImageLayout = info_in_pass->resolveImageLayout;

			return rendering_attachment_info;
		}





		[[nodiscard]] DynamicRenderingAttachmentFormatInfo GetDynamicRenderingAttachmentFormat(VkRsrcUsageInfoRGUniversalTex& tex_usage_and_tex)
		{
			//Only valid for an Attachment
			assert(Vk::RsrcType::Attachment == tex_usage_and_tex.tex_usage->GetRsrcType());

			const auto& usage_in_pass = *tex_usage_and_tex.tex_usage;
			auto attachment_index = std::get<Vk::AttachmentIndix>(usage_in_pass.binding_info);
			return { usage_in_pass.attach_type, usage_in_pass.format, attachment_index };
		}



	}
}        // namespace Anni
