#include "RsrcUsage.h"


Anni::RenderGraphV1::IRsrcUsage::IRsrcUsage(RsrcType rsrc_t_, RsrcAccessTypeRG access_t_) :
	rsrc_type(rsrc_t_), access_type(access_t_)
{
}

Anni::RenderGraphV1::IRsrcUsage::IRsrcUsage(RsrcType rsrc_t_, RsrcAccessTypeRG access_t_, RsrcOrigin origin_,
                                            std::shared_ptr<BinarySemWrapper> available_semaphore_,
                                            std::shared_ptr<BinarySemWrapper> finished_using_semaphore_) :
	rsrc_type(rsrc_t_), access_type(access_t_), origin(origin_)
{
}

Anni::RenderGraphV1::IRsrcUsage::~IRsrcUsage()
{
	
}

Anni::RsrcType Anni::RenderGraphV1::IRsrcUsage::GetRsrcType() const
{
	return rsrc_type;
}

Anni::RsrcAccessTypeRG Anni::RenderGraphV1::IRsrcUsage::GetRsrcAccessType() const
{
	return access_type;
}

Anni::RenderGraphV1::IRsrcUsage::RsrcOrigin Anni::RenderGraphV1::IRsrcUsage::GetRsrcOrigin() const
{
	return origin;
}

void Anni::RenderGraphV1::IRsrcUsage::SetRsrcOrigin(RsrcOrigin origin_)
{
	origin = origin_;
}

Anni::RenderGraphV1::TexUsage::TexUsage(const ImgSyncInfo& sync_info_):
	IRsrcUsage(RsrcType::ModelTextures, RsrcAccessTypeRG::Read), format(), desc_info(),
	sync_info(sync_info_)
{
}

Anni::RenderGraphV1::TexUsage::TexUsage(vk::Format format_, const DescriptorInfo& desc_info_,
                                        const ImgSyncInfo& sync_info_, RsrcAccessTypeRG access_t,
                                        const std::optional<vk::ImageViewCreateInfo>& img_view_CI_,
                                        const std::optional<vk::SamplerCreateInfo>& sampler_CI_,
                                        std::shared_ptr<BinarySemWrapper> available_semaphore_,
                                        std::shared_ptr<BinarySemWrapper> finished_using_semaphore_):
	IRsrcUsage(RsrcType::Texture, access_t),
	format(format_),
	img_view_CI(img_view_CI_),
	sampler_CI(sampler_CI_),
	desc_info(desc_info_),
	sync_info(sync_info_)
{
}

Anni::ImgSyncInfo& Anni::RenderGraphV1::TexUsage::GetSynInfo()
{
	return sync_info;
}

Anni::RenderGraphV1::AttachUsage::AttachUsage(vk::Format format_, const AttachmentInfo& attach_info_,
                                              const ImgSyncInfo& sync_info_, RsrcAccessTypeRG access_t_,
                                              const std::optional<VkImageViewCreateInfo>& img_view_CI_,
                                              const std::optional<VkSamplerCreateInfo>& sampler_CI_,
                                              std::shared_ptr<BinarySemWrapper> available_semaphore_,
                                              std::shared_ptr<BinarySemWrapper> finished_using_semaphore_):
	IRsrcUsage(RsrcType::Attachment, access_t_),
	format(format_),
	img_view_CI(img_view_CI_),
	sampler_CI(sampler_CI_),
	attach_info(attach_info_),
	sync_info(sync_info_)
{
}

Anni::ImgSyncInfo& Anni::RenderGraphV1::AttachUsage::GetSynInfo()
{
	return sync_info;
}

vk::RenderingAttachmentInfo Anni::RenderGraphV1::AttachUsage::GetVkRenderingAttachmentInfo() const
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

Anni::DynamicRenderingAttachmentFormatInfo Anni::RenderGraphV1::AttachUsage::
GetDynamicRenderingAttachmentFormatInfo() const
{
	return DynamicRenderingAttachmentFormatInfo(attach_info.attach_type, format, attach_info.attachment_index);
}

void Anni::RenderGraphV1::AttachUsage::BindResolveTarget(std::shared_ptr<ImgViewWrapper> resolve_img_view_,
                                                         vk::ImageLayout resolve_target_layout_)
{
	resolve_img_view = std::move(resolve_img_view_);
	resolve_target_layout = resolve_target_layout_;
}

Anni::RenderGraphV1::BufUsage::BufUsage(const DescriptorInfo& desc_info_, const BufSyncInfo& sync_info_,
                                        RsrcAccessTypeRG access_t_,
                                        std::shared_ptr<BinarySemWrapper> available_semaphore_,
                                        std::shared_ptr<BinarySemWrapper> finished_using_semaphore_):
	IRsrcUsage(RsrcType::Buffer, access_t_),
	desc_info(desc_info_),
	sync_info(sync_info_)
{
}

Anni::RenderGraphV1::BufUsage::BufUsage(RsrcType rsrc_type_, const BufSyncInfo& sync_info_, RsrcAccessTypeRG access_t_,
                                        std::shared_ptr<BinarySemWrapper> available_semaphore_,
                                        std::shared_ptr<BinarySemWrapper> finished_using_semaphore_):
	IRsrcUsage(rsrc_type_, access_t_),
	desc_info(),
	sync_info(sync_info_)
{
}
