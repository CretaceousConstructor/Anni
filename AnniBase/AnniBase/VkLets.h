#pragma once
#include "VkVirtualResource.h"
namespace Anni
{
	//namespace RenderGraphV0
	//{
	//	class PassNode;
	//}

	//namespace RenderGraphV0
	//{
	//	//TODO: concept约束U和R的类型
	//	//这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
	//	//这里面R参数（rsrc）表示是资源类型，其是传引用进入函数的。

	//	template <typename R, typename U>
	//	class RsrcInlet;

	//	template <typename R, typename U>
	//	class RsrcOutlet
	//	{
	//	public:
	//		RsrcOutlet(
	//			typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr_,
	//			RenderGraphV0::PassNode* pass_attached_to_,
	//			U* usage_);
	//		RsrcOutlet() = delete;

	//		typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetUnderlyingRsrcItr();

	//		std::optional<std::pair<RenderGraphV0::PassNode*, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>>
	//			GetProvidingOutletItrAndPass();
	//		void      AddAccessingPassAndItr(PassNode* pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr);
	//		void      AddAccessingPassAndItr(PassNode* pass, std::unordered_map<std::string, RsrcInlet<R, U>>::iterator itr);
	//		void      AssignProvidingPassAndProvidingOutItr(PassNode* pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr);
	//		PassNode* GetPassAttachedTo() const;
	//		U* GetUsage();

	//		std::vector<std::pair<RenderGraphV0::PassNode*, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> target_outlet_itrs;
	//		std::vector<std::pair<RenderGraphV0::PassNode*, std::unordered_map<std::string, RsrcInlet<R, U>>::iterator>>  target_inlet_itrs;

	//	private:
	//		typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr;
	//		U* usage;

	//		RenderGraphV0::PassNode* pass_attached_to;
	//		std::optional<std::pair<RenderGraphV0::PassNode*, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> providing_pass_and_source_outlet_itr;
	//	};

	//	template <typename R, typename U>
	//	class RsrcInlet
	//	{
	//	private:
	//		using VrsrcMapItr = typename std::unordered_map<std::string, VirtualResource<R>>::iterator;

	//	public:
	//		RsrcInlet(
	//			VrsrcMapItr              underlying_rsrc_itr_,
	//			RenderGraphV0::PassNode* pass_attached_to_,
	//			U* usage_);

	//		RsrcInlet(
	//			std::vector<VrsrcMapItr> underlying_rsrc_itrs_,
	//			RenderGraphV0::PassNode* pass_attached_to_,
	//			U* usage_) :
	//			model_tex_vrsrc(std::move(underlying_rsrc_itrs_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
	//		{
	//		}

	//		RsrcInlet() = delete;

	//		VrsrcMapItr                                                                                                               GetUnderlyingRsrcItr();
	//		std::optional<std::pair<RenderGraphV0::PassNode*, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> GetProvidingOutletItrAndPass();
	//		void                                                                                                                      AssignProvidingPassAndProvidingOutItr(PassNode* pass, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr);

	//		PassNode* GetPassAttachedTo() const;

	//		U* GetUsage();

	//		std::optional<std::pair<RenderGraphV0::PassNode*, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> GetProvidingOutletItrAndPass();

	//		//typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr;
	//		VrsrcMapItr underlying_rsrc_itr;
	//		U* usage;

	//		RenderGraphV0::PassNode* pass_attached_to;
	//		std::optional<std::pair<RenderGraphV0::PassNode*, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> providing_pass_and_source_outlet_itr;

	//		std::vector<VrsrcMapItr> model_tex_vrsrc;
	//	};

	//	template <typename R, typename U>
	//	inline RsrcOutlet<R, U>::RsrcOutlet(typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr_, RenderGraphV0::PassNode* pass_attached_to_, U* usage_) :
	//		underlying_rsrc_itr(std::move(underlying_rsrc_itr_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
	//	{
	//	}

	//	template <typename R, typename U>
	//	inline typename std::unordered_map<std::string, VirtualResource<R>>::iterator RsrcOutlet<R, U>::GetUnderlyingRsrcItr()
	//	{
	//		return underlying_rsrc_itr;
	//	}

	//	template <typename R, typename U>
	//	inline std::optional<std::pair<RenderGraphV0::PassNode*, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> RsrcOutlet<R, U>::GetProvidingOutletItrAndPass()
	//	{
	//		return providing_pass_and_source_outlet_itr;
	//	}

	//	template <typename R, typename U>
	//	inline void RsrcOutlet<R, U>::AddAccessingPassAndItr(PassNode* pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr)
	//	{
	//		target_outlet_itrs.emplace(pass, itr);
	//	}

	//	template <typename R, typename U>
	//	void RsrcOutlet<R, U>::AddAccessingPassAndItr(PassNode* pass, std::unordered_map<std::string, RsrcInlet<R, U>>::iterator itr)
	//	{
	//		target_inlet_itrs.emplace(pass, itr);
	//	}

	//	template <typename R, typename U>
	//	void RsrcOutlet<R, U>::AssignProvidingPassAndProvidingOutItr(PassNode* pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr)
	//	{
	//		providing_pass_and_source_outlet_itr.emplace(pass, itr);
	//	}

	//	template <typename R, typename U>
	//	PassNode* RsrcOutlet<R, U>::GetPassAttachedTo() const
	//	{
	//		return pass_attached_to;
	//	}

	//	template <typename R, typename U>
	//	U* RsrcOutlet<R, U>::GetUsage()
	//	{
	//		return usage;
	//	}

	//	template <typename R, typename U>
	//	inline RsrcInlet<R, U>::RsrcInlet(VrsrcMapItr underlying_rsrc_itr_, RenderGraphV0::PassNode* pass_attached_to_, U* usage_) :
	//		underlying_rsrc_itr(std::move(underlying_rsrc_itr_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
	//	{
	//	}

	//	template <typename R, typename U>
	//	inline RsrcInlet<R, U>::RsrcInlet(
	//		std::vector<VrsrcMapItr> underlying_rsrc_itrs_,
	//		RenderGraphV0::PassNode* pass_attached_to_,
	//		U* usage_) :
	//		model_tex_vrsrc(std::move(underlying_rsrc_itrs_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
	//	{
	//	}

	//	template <typename R, typename U>
	//	inline typename std::unordered_map<std::string, VirtualResource<R>>::iterator RsrcInlet<R, U>::GetUnderlyingRsrcItr()
	//	{
	//		return underlying_rsrc_itr;
	//	}

	//	template <typename R, typename U>
	//	inline void RsrcInlet<R, U>::AssignProvidingPassAndProvidingOutItr(PassNode* pass, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr)
	//	{
	//		providing_pass_and_source_outlet_itr.emplace(pass, itr);
	//	}

	//	template <typename R, typename U>
	//	inline PassNode* RsrcInlet<R, U>::GetPassAttachedTo() const
	//	{
	//		return pass_attached_to;
	//	}

	//	template <typename R, typename U>
	//	inline U* RsrcInlet<R, U>::GetUsage()
	//	{
	//		return usage;
	//	}

	//	template <typename R, typename U>
	//	inline std::optional<std::pair<RenderGraphV0::PassNode*, typename std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> RsrcInlet<R, U>::GetProvidingOutletItrAndPass()
	//	{
	//		return providing_pass_and_source_outlet_itr;
	//	}

	//}        // namespace RenderGraphV0
}        // namespace Anni
