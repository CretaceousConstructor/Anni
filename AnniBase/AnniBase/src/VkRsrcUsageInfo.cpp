
#include "VkRsrcUsageInfo.h"
namespace Anni
{
	//RsrcUsageInfoInPass::~RsrcUsageInfoInPass() = default;

	//RsrcUsageInfoInPass::RsrcUsageInfoInPass(Vk::RsrcType rsrc_t_, Vk::RsrcAccessTypeRG access_t_) :
	//	rsrc_type(rsrc_t_), access_type(access_t_)
	//{
	//}

	//RsrcUsageInfoInPass::RsrcUsageInfoInPass(Vk::RsrcType rsrc_t_, Vk::RsrcAccessTypeRG access_t_, RsrcOrigin origin_) :
	//	rsrc_type(rsrc_t_), access_type(access_t_), origin(origin_)
	//{
	//}

	//Vk::RsrcType RsrcUsageInfoInPass::GetRsrcType() const
	//{
	//	return rsrc_type;
	//}

	//Vk::RsrcAccessTypeRG RsrcUsageInfoInPass::GetRsrcAccessType() const
	//{
	//	return access_type;
	//}

	//RsrcUsageInfoInPass::RsrcOrigin RsrcUsageInfoInPass::GetRsrcOrigin() const
	//{
	//	return origin;
	//}

	//void RsrcUsageInfoInPass::SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin origin)
	//{
	//	origin = origin;
	//}

	//VkAttachmentInfo::Bundle::Bundle(std::vector<VkAttachmentInfo> info_array_, DynamicRenderingAttachmentFormatInfo attachment_format_) :
	//	info_array(std::move(info_array_)),
	//	attachment_format(attachment_format_)
	//{
	//}

	//DynamicRenderingAttachmentFormatInfo VkAttachmentInfo::Bundle::GetAttachmentFormatAndType() const
	//{
	//	return attachment_format;
	//}

	//VkAttachmentInfo& VkAttachmentInfo::Bundle::operator[](size_t index)
	//{
	//	return info_array[index];
	//}

	//VkAttachmentInfo::VkAttachmentInfo(const WithinPassRG& info, std::shared_ptr<VkTexture> tex_) :

	//	info_in_pass(info),
	//	tex(tex_)

	//{
	//}

	//VkAttachmentInfo::VkAttachmentInfo(const WithinPassRG& info, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_) :

	//	info_in_pass(info),
	//	tex(tex_),
	//	resolve_tex(resolve_tex_)

	//{
	//}

	//VkAttachmentInfo::VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_) :
	//	info_in_pass(meme_),
	//	tex(tex_)
	//{
	//}

	//VkAttachmentInfo::VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_) :
	//	info_in_pass(meme_),
	//	tex(tex_),
	//	resolve_tex(resolve_tex_)
	//{
	//}

	//VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const Memento &meme_)
	//{
	//
	//	std::vector<VkAttachmentInfo> result;
	//
	//	result.emplace_back(meme_);
	//	return {result, {meme_.type, meme_.format, meme_.attachment_index}};
	//}

	//VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const WithinPass& meme_, const VkTexture::TexturePtrBundle& textures_)
	//{
	//	std::vector<VkAttachmentInfo> result;

	//	for (const auto& tex : textures_)
	//	{
	//		result.emplace_back(meme_, tex);
	//	}
	//	return { result, {meme_.type, meme_.format, meme_.attachment_index} };
	//}

	//VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const WithinPass& meme_, const VkTexture::TexturePtrBundle& textures_, const VkTexture::TexturePtrBundle& resolve_textures_)
	//{
	//	std::vector<VkAttachmentInfo> result;

	//	assert(textures_.size() == resolve_textures_.size());

	//	auto tex_itr = textures_.begin();
	//	auto resolve_tex_itr = resolve_textures_.begin();
	//	for (; (tex_itr != textures_.end()) && (resolve_tex_itr != resolve_textures_.end()); (++tex_itr, ++resolve_tex_itr))
	//	{
	//		result.emplace_back(meme_, *tex_itr, *resolve_tex_itr);
	//	}

	//	return { result, {meme_.type, meme_.format, meme_.attachment_index} };
	//}

	//const VkAttachmentInfo::WithinPass& VkAttachmentInfo::GetInfo() const
	//{
	//	assert(info_in_pass);
	//	return *info_in_pass;
	//}

	//DynamicRenderingAttachmentFormatInfo VkAttachmentInfo::GetAttachmentFormatAndType() const
	//{
	//	if (!info_in_pass)
	//	{
	//		throw std::runtime_error("incomplete attachment info!");
	//	}
	//	return { info_in_pass->type, info_in_pass->format, info_in_pass->attachment_index };
	//}

	//void VkAttachmentInfo::SetMemento(WithinPass meme_)
	//{
	//	info_in_pass = meme_;
	//}

	//void VkAttachmentInfo::SetTexture(std::shared_ptr<VkTexture> tex_)
	//{
	//	tex = tex_;
	//}

	//const VkTexture& VkAttachmentInfo::GetTex() const
	//{
	//	return *tex;
	//}

	//VkTexture& VkAttachmentInfo::GetTex()
	//{
	//	return *tex;
	//}

	//VkRenderingAttachmentInfo VkAttachmentInfo::GetRenderingAttachmentInfo()
	//{
	//	if (!info_in_pass || !tex)
	//	{
	//		throw std::runtime_error("incomplete attachment info!");
	//	}

	//	rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	//	rendering_attachment_info.imageView = tex->GetTextureImageView().GetRawImgView();
	//	rendering_attachment_info.imageLayout = info_in_pass->layout_inpass;
	//	rendering_attachment_info.loadOp = info_in_pass->loadOp;
	//	rendering_attachment_info.storeOp = info_in_pass->storeOp;
	//	rendering_attachment_info.clearValue = info_in_pass->clear_value;

	//	rendering_attachment_info.resolveMode = info_in_pass->resolveMode;

	//	if (resolve_tex)
	//	{
	//		rendering_attachment_info.resolveImageView = resolve_tex->GetTextureImageView().GetRawImgView();
	//	}

	//	rendering_attachment_info.resolveImageLayout = info_in_pass->resolveImageLayout;

	//	return rendering_attachment_info;
	//}

	//use as texture
	//VkUniversalTexUsageInfoRG::VkUniversalTexUsageInfoRG(
	//	VkFormat                                           format_,
	//	std::variant<Vk::AttachmentIndix, Vk::DescSetInfo> binding_info_,

	//	VkAccessFlags        access_mask_,
	//	VkPipelineStageFlags pip_stage_mask_,
	//	VkImageLayout        layout_inpass_,

	//	VkDescriptorType     type_,
	//	VkShaderStageFlags   shader_stages_,
	//	Vk::RsrcAccessTypeRG access_t,

	//	std::optional<VkSamplerCreateInfo>   sampler_CI_,
	//	std::optional<VkImageViewCreateInfo> img_view_CI_) :
	//	RsrcUsageInfoInPass(Vk::RsrcType::Texture, access_t),
	//	format(format_),
	//	sampler_CI(sampler_CI_),
	//	img_view_CI(img_view_CI_),
	//	binding_info(binding_info_),
	//	access_mask(access_mask_),
	//	pip_stage_mask(pip_stage_mask_),
	//	layout_inpass(layout_inpass_),
	//	tex_type(type_),
	//	shader_stages(shader_stages_)
	//{
	//}

	////used for model texture importing
	//VkUniversalTexUsageInfoRG::VkUniversalTexUsageInfoRG(
	//	VkAccessFlags        access_mask_,
	//	VkPipelineStageFlags pip_stage_mask_,
	//	VkImageLayout        layout_inpass_,

	//	VkDescriptorType   type_,
	//	VkShaderStageFlags shader_stages_) :

	//	RsrcUsageInfoInPass(Vk::RsrcType::Texture, Vk::RsrcAccessTypeRG::Read),
	//	access_mask(access_mask_),
	//	pip_stage_mask(pip_stage_mask_),
	//	layout_inpass(layout_inpass_),

	//	tex_type(type_),
	//	shader_stages(shader_stages_)
	//{
	//}

	////use as attachment
	//VkUniversalTexUsageInfoRG::VkUniversalTexUsageInfoRG(
	//	VkFormat                                           format_,
	//	std::variant<Vk::AttachmentIndix, Vk::DescSetInfo> binding_info_,

	//	VkAccessFlags        access_mask_,
	//	VkPipelineStageFlags pip_stage_mask_,
	//	VkImageLayout        layout_inpass_,

	//	AttachmentType       attach_type_,
	//	VkClearValue         clear_value_,
	//	Vk::RsrcAccessTypeRG access_t,

	//	VkResolveModeFlagBits resolve_mode_ = VK_RESOLVE_MODE_NONE,
	//	VkImageLayout         resolve_image_layout_ = VK_IMAGE_LAYOUT_UNDEFINED,
	//	//VkImageView              resolveImageView{};
	//	VkAttachmentLoadOp                   load_op_ = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	//	VkAttachmentStoreOp                  store_op_ = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	//	std::optional<VkSamplerCreateInfo>   sampler_CI_,
	//	std::optional<VkImageViewCreateInfo> img_view_CI_) :

	//	RsrcUsageInfoInPass(Vk::RsrcType::Attachment, access_t),
	//	format(format_),
	//	sampler_CI(sampler_CI_),
	//	img_view_CI(img_view_CI_),
	//	binding_info(binding_info_),
	//	access_mask(access_mask_),
	//	pip_stage_mask(pip_stage_mask_),
	//	layout_inpass(layout_inpass_),
	//	attach_type(attach_type_),
	//	clear_value(clear_value_),
	//	resolve_mode(resolve_mode_),
	//	resolve_image_layout(resolve_image_layout_),
	//	load_op(load_op_),
	//	store_op(store_op_)

	//{
	//}



	//std::optional<VkSamplerCreateInfo> VkUniversalTexUsageInfoRG::GetSamplerCI() const
	//{
	//	return sampler_CI;
	//}

	//std::optional<VkImageViewCreateInfo> VkUniversalTexUsageInfoRG::GetImgViewCI() const
	//{
	//	return img_view_CI;
	//}

	//Vk::TexSyncInfo VkUniversalTexUsageInfoRG::GetTexSynInfo() const
	//{
	//	return {
	//		.access_mask = access_mask,
	//		.stage_mask = pip_stage_mask,
	//		.layout_inpass = layout_inpass,
	//		.img_subrange = subrange };
	//}

	///// <summary>
	/////	Buffer usage
	///// </summary>
	///// <param name="slot_info_"></param>
	///// <param name="buf_type_"></param>

	///// <param name="access_mask_"></param>
	///// <param name="shader_stages_"></param>
	///// <param name="access_t"></param>
	///// <param name="subrange_"></param>

	//VkBufUsageInfoRG::VkBufUsageInfoRG(
	//	Vk::DescSetInfo  slot_info_,
	//	VkDescriptorType buf_type_,

	//	VkAccessFlags      access_mask_,
	//	VkShaderStageFlags shader_stages_,

	//	Vk::RsrcAccessTypeRG          access_t,
	//	std::optional<BufferSubRange> subrange_) :
	//	RsrcUsageInfoInPass(Vk::RsrcType::Buffer, access_t),
	//	set_info(slot_info_),
	//	buf_type(buf_type_),
	//	access_mask(access_mask_),
	//	shader_stages(shader_stages_)
	//{
	//	if (subrange_)
	//	{
	//		subrange = *subrange_;
	//	}
	//	else
	//	{
	//		subrange = BufferSubRange{ .offset = 0, .size = VK_WHOLE_SIZE };
	//	}
	//}

	//VkBufUsageInfoRG::VkBufUsageInfoRG(const VkBufUsageInfoRG& other) :
	//	RsrcUsageInfoInPass(other.rsrc_type, other.access_type),

	//	set_info(other.set_info),
	//	buf_type(other.buf_type),

	//	access_mask(other.access_mask),
	//	shader_stages(other.shader_stages),
	//	subrange(other.subrange)
	//{
	//}

	//Vk::BufSyncInfo VkBufUsageInfoRG::GetBufSynInfo() const
	//{
	//	VkDeviceSize offset = subrange ? subrange.value().offset : 0u;
	//	VkDeviceSize size = subrange ? subrange.value().size : VK_WHOLE_SIZE;

	//	subrange.value().size;
	//	Vk::BufSyncInfo{
	//		.access_mask = access_mask,
	//		.stage_mask = shader_stages,
	//		.offset = offset,
	//		.size = size };
	//}



}        // namespace Anni
