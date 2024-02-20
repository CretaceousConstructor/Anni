#pragma once
#include "AnniVkHeader.h"

namespace Anni
{
	enum struct AttachmentType
	{
		Unknown,
		ColorAttachment,
		DepthAttachment,
		DepthStencilAttachment,
		//StencilAttachment, // reserved for future use
		ResolveOpTargetAttachment,
		NotUsedWithinPass

	};

	struct AttachmentInfo
	{
		uint32_t attachment_index;
		AttachmentType      attach_type;
		vk::ClearValue clear_value;


		vk::ResolveModeFlagBits resolve_mode{ vk::ResolveModeFlagBits::eNone };
		vk::ImageLayout resolve_image_layout{ vk::ImageLayout::eUndefined };

		vk::AttachmentLoadOp load_op{ vk::AttachmentLoadOp::eDontCare };
		vk::AttachmentStoreOp store_op{ vk::AttachmentStoreOp::eDontCare };

	};




	struct DynamicRenderingAttachmentFormatInfo
	{
	public:
		explicit DynamicRenderingAttachmentFormatInfo() = default;


		DynamicRenderingAttachmentFormatInfo(
			AttachmentType     attach_type_,
			vk::Format format_,
			uint32_t index_
		);
		//
		vk::Format format;
		//attach type only used to distribute to different attachment vector(may it be depth attach ,color attach etc.)
		AttachmentType attach_type;
		//index ONLY use to sort DynamicRenderingAttachmentFormats, no any other usages.
		uint32_t index;
	};




}
