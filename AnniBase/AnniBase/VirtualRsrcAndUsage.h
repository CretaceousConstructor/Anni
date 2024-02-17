#pragma once
#include "VirtualBuffer.h"
#include "VirtualTexture.h"
#include "RsrcUsage.h"
#include <variant>


namespace Anni::RenderGraphV1
{
	using VBufItr = std::unordered_map<std::string, VirtualBuffer>::iterator;
	using VTexItr = std::unordered_map<std::string, VirtualTexture>::iterator;

	class VirtualBufRsrcAndUsage
	{
	public:
		VirtualBufRsrcAndUsage(BufUsage buf_usage_, VBufItr buf_itr_) :
			usage(buf_usage_), v_rsrc(buf_itr_)
		{

		}
		BufUsage                                                           usage;
		VBufItr                                                            v_rsrc;
	};

	class VirtualTexRsrcAndUsage
	{
	public:
		VirtualTexRsrcAndUsage(std::variant<TexUsage, AttachUsage> tex_usage_, VTexItr tex_itr_) :
			usage(tex_usage_), v_rsrc(tex_itr_)
		{

		}
		void PreBindResolveTarget(VirtualTexRsrcAndUsage& resovel_target_)
		{
			resolve_target = &resovel_target_;
		}
		void BindResolveTarget()
		{
			ASSERT_WITH_MSG(resolve_target, "No resolve target specified!"); 
			VirtualTexRsrcAndUsage* p_resovel_target = resolve_target.value();
			const auto& resovel_target_usage = std::get<AttachUsage>(p_resovel_target->usage);
			auto& cur_attach_usage = std::get<AttachUsage>(this->usage);
			cur_attach_usage.BindResolveTarget(resovel_target_usage.img_view, resovel_target_usage.sync_info.layout_inpass);
		}



		std::variant<TexUsage, AttachUsage>                            usage;
		VTexItr v_rsrc;

		std::optional<VirtualTexRsrcAndUsage*>         resolve_target;
	};
}

