#include "VirtualRsrcAndUsage.h"

Anni::RenderGraphV1::VirtualBufRsrcAndUsage::VirtualBufRsrcAndUsage(const BufUsage& buf_usage_, VBufItr buf_itr_):
	usage(buf_usage_), v_rsrc(buf_itr_)
{

}

Anni::RenderGraphV1::VirtualTexRsrcAndUsage::VirtualTexRsrcAndUsage(
	const std::variant<TexUsage, AttachUsage>& tex_usage_, VTexItr tex_itr_) :
	usage(tex_usage_), v_rsrc(tex_itr_)
{

}

void Anni::RenderGraphV1::VirtualTexRsrcAndUsage::PreBindResolveTarget(VirtualTexRsrcAndUsage& resovel_target_)
{
	resolve_target = &resovel_target_;
}

void Anni::RenderGraphV1::VirtualTexRsrcAndUsage::BindResolveTarget()
{
	ASSERT_WITH_MSG(resolve_target, "No resolve target specified!");
	const VirtualTexRsrcAndUsage* const p_resovel_target = resolve_target.value();
	const auto& resovel_target_usage = std::get<AttachUsage>(p_resovel_target->usage);
	auto& cur_attach_usage = std::get<AttachUsage>(this->usage);
	cur_attach_usage.BindResolveTarget(resovel_target_usage.img_view, resovel_target_usage.sync_info.layout_inpass);
}
