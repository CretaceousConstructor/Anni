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
			GraphicsComponent& gfx_,
			vk::Image          image_,
			ImageCIEnhanced&    image_CI_
			);

		virtual ~ImageBaseReFac() = default;

		ImageBaseReFac(const ImageBaseReFac&) = delete;
		ImageBaseReFac& operator=(const ImageBaseReFac&) = delete;

		ImageBaseReFac(ImageBaseReFac&&) = delete;
		ImageBaseReFac& operator=(ImageBaseReFac&&) = delete;

		[[nodiscard]] vk::Image GetImage() const;

		[[nodiscard]] vk::Format GetImageFormat() const;

		[[nodiscard]] const ImageCIEnhanced& GetCI() const;

		std::shared_ptr<TimelineSemWrapper>& GetSemUsedToTransfer();

	public:
		//void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const DeviceManager::CommandPoolType command_type, std::optional<VkImageSubresourceRange> subresource_range) const;

		//�������Ŀǰû�б�ʹ�ã���Ϊ�����û�ȡ����������ôдimage barrier��������ͨ��һ��if elseȥ���衣
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
