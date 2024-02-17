#include "AttachmentInfo.h"
namespace Anni
{
	DynamicRenderingAttachmentFormatInfo::DynamicRenderingAttachmentFormatInfo(
		AttachmentType     attach_type_,
		vk::Format format_,
		uint32_t index_)
		:
		attach_type(attach_type_),
		format(format_),
		index(index_)
	{
	}

}
