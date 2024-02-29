#pragma once
#include <unordered_map>
#include <optional>
#include <variant>
#include "VirtualBuffer.h"
#include "VirtualTexture.h"
#include "RsrcUsage.h"


namespace Anni::RenderGraphV1
{
	class GraphicsPassNode;
}

namespace Anni::RenderGraphV1
{
	template <typename U>
	concept UsageT = std::is_same_v<U, BufUsage> || std::is_same_v<U, std::variant<TexUsage, AttachUsage>>;


	template <UsageT U>
	struct HandleType
	{
		//using type = void; // Default case
	};

	template <>
	struct HandleType<BufUsage>
	{
		using type = VirtualBuffer::Handle;
	};

	template <>
	struct HandleType<std::variant<TexUsage, AttachUsage>>
	{
		using type = VirtualTexture::Handle;
	};


	template <typename Usage, typename RsrcHandle>
	class RsrcInlet;

	template <typename Usage, typename RsrcHandle>
	class RsrcOutlet;


	template <typename Usage, typename VRsrcHandle = typename HandleType<Usage>::type>
	class RsrcOutlet
	{
	public:
		struct Handle
		{
			Handle() = delete;

			explicit Handle(uint64_t handle_, GraphicsPassNode* pass_attached_to_)
				:
				handle(handle_), pass_attached_to(pass_attached_to_)

			{
			}

			uint64_t handle;
			GraphicsPassNode* pass_attached_to;
		};

		RsrcOutlet(
			Usage usage_,
			VRsrcHandle rsrc_handle_,
			GraphicsPassNode* pass_attached_to_
		)
			:
			usage(usage_),
			rsrc_handle(rsrc_handle_),
			pass_attached_to(pass_attached_to_)
		{
		}


		RsrcOutlet() = delete;
		RsrcOutlet(const RsrcOutlet&) = delete;
		RsrcOutlet(RsrcOutlet&&) = default;

		RsrcOutlet& operator=(const RsrcOutlet&) = delete;
		RsrcOutlet& operator=(RsrcOutlet&&) = default;
		~RsrcOutlet() = default;


		VRsrcHandle GetUnderlyingRsrcHandle()
		{
			return rsrc_handle;
		}

		std::optional<Handle>& GetProvidingOutlet()
		{
			return source_outlet;
		}

		void AssignProvidingOutlet(Handle providing_outlet_handle)
		{
			source_outlet.emplace(providing_outlet_handle);
		}

		GraphicsPassNode* GetPassAttachedTo() const
		{
			return pass_attached_to;
		}

		Usage& GetRsrcUsage()
		{
			return usage;
		}


		Usage usage;
		VRsrcHandle rsrc_handle;
		GraphicsPassNode* pass_attached_to;
		std::optional<Handle> source_outlet;


		std::variant<std::monostate,
		             Handle,
		             typename RsrcInlet<Usage, VRsrcHandle>::Handle>
		resolve_target_let;
	};


	template <typename Usage, typename VRsrcHandle = typename HandleType<Usage>::type>
	class RsrcInlet
	{
	public:
		struct Handle
		{
			Handle() = delete;

			explicit Handle(uint64_t handle_, GraphicsPassNode* pass_attached_to_)
				:
				handle(handle_), pass_attached_to(pass_attached_to_)

			{
			}

			uint64_t handle;
			GraphicsPassNode* pass_attached_to;
		};


		RsrcInlet(
			Usage usage_,
			VRsrcHandle rsrc_handle_,
			GraphicsPassNode* pass_attached_to_
		)
			:
			usage(usage_),
			rsrc_handle(rsrc_handle_),
			pass_attached_to(pass_attached_to_)
		{
		}

		RsrcInlet() = delete;
		RsrcInlet(const RsrcInlet&) = delete;
		RsrcInlet(RsrcInlet&&) = default;

		RsrcInlet& operator=(const RsrcInlet&) = delete;
		RsrcInlet& operator=(RsrcInlet&&) = default;
		~RsrcInlet() = default;

		VRsrcHandle GetUnderlyingRsrcHandle()
		{
			return rsrc_handle;
		}

		std::optional<typename RsrcOutlet<Usage, VRsrcHandle>::Handle>& GetProvidingOutlet()
		{
			return source_outlet;
		}

		void AssignProvidingOutlet(typename RsrcOutlet<Usage, VRsrcHandle>::Handle providing_outlet_handle)
		{
			source_outlet.emplace(providing_outlet_handle);
		}

		GraphicsPassNode* GetPassAttachedTo() const
		{
			return pass_attached_to;
		}

		Usage& GetRsrcUsage()
		{
			return usage;
		}

		Usage usage;
		VRsrcHandle rsrc_handle;
		GraphicsPassNode* pass_attached_to;
		std::optional<typename RsrcOutlet<Usage, VRsrcHandle>::Handle> source_outlet;

		std::variant<std::monostate,
		             typename RsrcOutlet<Usage, VRsrcHandle>::Handle,
		             Handle>
		resolve_target_let;
	};
}
