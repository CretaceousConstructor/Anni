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


	public:
		RsrcOutlet(
			RU& vRsrcAndUsage,
			GraphicsPassNode* pass_attached_to_
		)
			:
			vrsrc_usage(vRsrcAndUsage), pass_attached_to(pass_attached_to_)
		{
		}


		//typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetUnderlyingRsrcItr();
		decltype(auto) GetUnderlyingRsrcItr()
		{
			return vrsrc_usage.v_rsrc;
		}

		std::optional<std::pair<GraphicsPassNode*, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator>>  GetProvidingOutletItrAndPass()
		{

			return providing_pass_and_source_outlet_itr;
		}
		//void AddAccessingPassAndItr(GraphicsPassNode* pass, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator itr)
		//{
		//	target_outlet_itrs.emplace(pass, itr);
		//}
		//void AddAccessingPassAndItr(GraphicsPassNode* pass, std::unordered_map<std::string, RsrcInlet<RU>>::iterator itr)
		//{
		//	target_inlet_itrs.emplace(pass, itr);
		//}


		void AssignProvidingPassAndProvidingOutItr(GraphicsPassNode* pass, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator itr)
		{ 
			providing_pass_and_source_outlet_itr.emplace(pass, itr);
		}

		GraphicsPassNode* GetPassAttachedTo() const
		{
			return pass_attached_to;
		}
		decltype(auto) GetRsrcUsage()
		{
			return (vrsrc_usage.usage);
		}

		//std::vector<std::pair<GraphicsPassNode*, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator>> target_outlet_itrs;
		//std::vector<std::pair<GraphicsPassNode*, std::unordered_map<std::string, RsrcInlet<RU>>::iterator>>  target_inlet_itrs;

	private:
		RU& vrsrc_usage;
		GraphicsPassNode* pass_attached_to;
		std::optional<std::pair<GraphicsPassNode*, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator>> providing_pass_and_source_outlet_itr;
	};


	template <typename RU>
	class RsrcInlet
	{
	public:


	public:
		RsrcInlet(
			RU& vbufAndUsage,
			GraphicsPassNode* pass_attached_to_
		)
			:
			vrsrc_usage(vbufAndUsage), pass_attached_to(pass_attached_to_)
		{
		}
		//TODO:
				//RsrcInlet(
				//	std::vector<VrsrcMapItr> underlying_rsrc_itrs_,
				//	RenderGraphV0::PassNode* pass_attached_to_,
				//	U* usage_) :
				//	model_tex_vrsrc(std::move(underlying_rsrc_itrs_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
				//{
				//}

		RsrcInlet() = delete;

		decltype(auto) GetUnderlyingRsrcItr()
		{
			return vrsrc_usage.v_rsrc;
		}



		void AssignProvidingPassAndProvidingOutItr(GraphicsPassNode* pass, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator itr)
		{
			providing_pass_and_source_outlet_itr.emplace(pass, itr);
		}
		GraphicsPassNode* GetPassAttachedTo() const
		{
			return pass_attached_to;
		}


		decltype(auto) GetRsrcUsage()
		{
			return (vrsrc_usage.usage);
		}


		std::optional<std::pair<GraphicsPassNode*, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator>> GetProvidingOutletItrAndPass()
		{
			return providing_pass_and_source_outlet_itr;
		}

		RU& vrsrc_usage;
		GraphicsPassNode* pass_attached_to;
		std::optional<std::pair<GraphicsPassNode*, std::unordered_map<std::string, RsrcOutlet<RU>>::iterator>> providing_pass_and_source_outlet_itr;

		//std::vector<VrsrcMapItr> model_tex_vrsrc;
	};

}
