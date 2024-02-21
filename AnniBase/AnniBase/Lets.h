#pragma once
#include <unordered_map>
#include <optional>
#include "Lets.h"
#include "VirtualRsrcAndUsage.h"


namespace Anni::RenderGraphV1
{
	class GraphicsPassNode;
}

namespace Anni::RenderGraphV1
{

	template <typename RU>
	class RsrcInlet;

	template <typename RU>
	class RsrcOutlet;


	//R is VirtualBufRsrcAndUsage or  VirtualTexRsrcAndUsage
	//VirtualBufRsrcAndUsage
	template <typename RU>
	class RsrcOutlet
	{

	public:
		RsrcOutlet(
			RU& vRsrcAndUsage,
			GraphicsPassNode* pass_attached_to_
		)
			:
			vrsrc_usage(vRsrcAndUsage), pass_attached_to(pass_attached_to_)
		{
		}

		decltype(auto) GetUnderlyingRsrcItr()
		{
			return vrsrc_usage.v_rsrc;
		}

		std::optional<RsrcOutlet<RU>*>& GetProvidingOutlet()
		{
			return source_outlet;
		}

		void AssignProvidingOutlet(RsrcOutlet<RU>& source_outlet_)
		{
			source_outlet.emplace(&source_outlet_);
		}

		GraphicsPassNode* GetPassAttachedTo() const
		{
			return pass_attached_to;
		}

		decltype(auto) GetUsage()
		{
			return (vrsrc_usage.usage);
		}

		RU& GetRsrcAndUsage()
		{
			return vrsrc_usage;
		}


	private:
		RU& vrsrc_usage;
		GraphicsPassNode* pass_attached_to;
		std::optional<RsrcOutlet<RU>*> source_outlet;
	};


	template <typename RU>
	class RsrcInlet
	{
	public:
		RsrcInlet(
			RU& vbufAndUsage,
			GraphicsPassNode* pass_attached_to_
		)
			:
			vrsrc_usage(vbufAndUsage), pass_attached_to(pass_attached_to_)
		{
		}

		RsrcInlet() = delete;

		decltype(auto) GetUnderlyingRsrcItr()
		{
			return vrsrc_usage.v_rsrc;
		}

		void AssignProvidingOutlet(RsrcOutlet<RU>& source_outlet_)
		{
			source_outlet.emplace(&source_outlet_);
		}

		const GraphicsPassNode* GetPassAttachedTo() const
		{
			return pass_attached_to;
		}


		decltype(auto) GetRsrcUsage()
		{
			return (vrsrc_usage.usage);
		}


		std::optional<RsrcOutlet<RU>*>& GetProvidingOutlet()
		{
			return source_outlet;
		}

		RU& vrsrc_usage;
		GraphicsPassNode* pass_attached_to;
		std::optional<RsrcOutlet<RU>*> source_outlet;
	};

}
