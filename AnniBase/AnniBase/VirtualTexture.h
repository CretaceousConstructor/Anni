#pragma once
#include "IVirtualResource.h"
#include "VkTexture.h"


namespace Anni::RenderGraphV1
{




	class VirtualTexture : public IVirtualResource
	{
		friend class VkTextureFactory;
	public:

		struct Handle
		{
		  auto operator<=>(const Handle&) const = default;
		  Handle();

		  explicit Handle(uint64_t handle_);

		  uint64_t handle{~uint64_t(0x0)};
		};

		VirtualTexture(std::string name_, std::vector<std::shared_ptr<VkTexture>>* const p_rsrcs_);

		VirtualTexture(std::string name_, VkTexture::Descriptor descriptor_);

		VirtualTexture(std::string name_, std::shared_ptr<VkTexture> ptr_rsrc_);

		VirtualTexture() = delete;

		VirtualTexture(const VirtualTexture&) =default;
		VirtualTexture& operator=(const VirtualTexture& rhs) = delete;
		VirtualTexture(VirtualTexture&&) = default;
		VirtualTexture& operator=(VirtualTexture&&) = delete;
		~VirtualTexture() override = default;



		vk::ImageMemoryBarrier2 GetTexBarrier(const Anni::ImgSyncInfo& source_syn, const Anni::ImgSyncInfo& target_sync)const;

		std::shared_ptr<VkTexture>                    p_rsrc;
		std::optional<VkTexture::Descriptor>          descriptor;
		std::optional<std::vector<std::shared_ptr<VkTexture>>* const> p_rsrcs;

	};

	using VTexHandle = VirtualTexture::Handle;
}

