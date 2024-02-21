#pragma once
#include "VirtualBuffer.h"
#include "VirtualTexture.h"
#include "RsrcUsage.h"
#include <variant>


namespace Anni::RenderGraphV1
{

	class VirtualBufRsrcAndUsage
	{
	public:
		VirtualBufRsrcAndUsage(const BufUsage& buf_usage_, VBufItr buf_itr_);
		BufUsage                                                           usage;
		VBufItr                                                            v_rsrc;
	};

	class VirtualTexRsrcAndUsage
	{
	public:
		VirtualTexRsrcAndUsage(const std::variant<TexUsage, AttachUsage>& tex_usage_, VTexItr tex_itr_);
		void PreBindResolveTarget(VirtualTexRsrcAndUsage& resovel_target_);
		void BindResolveTarget();


		std::variant<TexUsage, AttachUsage>                            usage;
		VTexItr v_rsrc;

		std::optional<VirtualTexRsrcAndUsage*>         resolve_target;
	};
}

