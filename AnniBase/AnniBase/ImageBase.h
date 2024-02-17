#pragma once

#include "GraphicsComponent.h"
#include "ImageCI.h"
#include "Vk.h"

namespace Anni
{
	class ImageBaseReFac
	{
		friend class ImageFactory;

	public:
		ImageBaseReFac(
			GraphicsComponent& _gfx,
			vk::Image    image_,
			ImageCIEnhanced    image_CI_
			) :
			gfx(_gfx), device_manager(gfx.DeviceMan()), image(image_), vk_image_CI(image_CI_)
		{
		}

		virtual ~ImageBaseReFac() = default;

		ImageBaseReFac(const ImageBaseReFac&) = delete;
		ImageBaseReFac& operator=(const ImageBaseReFac&) = delete;

		ImageBaseReFac(ImageBaseReFac&&) = delete;
		ImageBaseReFac& operator=(ImageBaseReFac&&) = delete;

		[[nodiscard]] vk::Image GetImage() const
		{
			return image;
		}

		[[nodiscard]] vk::Format GetImageFormat() const
		{
			return vk_image_CI.vk_image_CI.format;
		}
		[[nodiscard]] const ImageCIEnhanced& GetCI() const
		{
			return vk_image_CI;
		}

		std::shared_ptr<TimelineSemWrapper>& GetSemUsedToTransfer()
		{
			return sem_used_to_transfer;
		}

	public:
		//void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const DeviceManager::CommandPoolType command_type, std::optional<VkImageSubresourceRange> subresource_range) const;

		//这个函数目前没有被使用，是为了让用户取解决具体该怎么写image barrier，而不是通过一堆if else去假设。
		//void TransitionImageLayout(Sync::VkImageMemoryBarrierEnhanced mem_barrier_enhanced, const DeviceManager::CommandPoolType command_type) const;

	protected:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;

	protected:
		vk::Image         image;
		ImageCIEnhanced vk_image_CI;

	protected:
		Queue* queue_used_to_tranfer{ nullptr };
		std::shared_ptr<TimelineSemWrapper> sem_used_to_transfer;


	};


}        // namespace Anni
