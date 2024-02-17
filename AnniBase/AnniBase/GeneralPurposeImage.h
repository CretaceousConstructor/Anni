#pragma once
#include "ImageBase.h"
namespace Anni
{
	class GeneralPurposeImageReFac final : public ImageBaseReFac
	{
		friend class ImageFactory;
	public:
		GeneralPurposeImageReFac(GraphicsComponent& gfx_, const vk::Image image_, const vk::DeviceMemory image_mem_, ImageCIEnhanced vk_image_CI,
			VkTimelineSemPoolUnsafe& sem_pool_
			);

		~GeneralPurposeImageReFac() override;
		GeneralPurposeImageReFac() = delete;

		GeneralPurposeImageReFac(const GeneralPurposeImageReFac&) = delete;
		GeneralPurposeImageReFac& operator=(const GeneralPurposeImageReFac&) = delete;

		GeneralPurposeImageReFac(GeneralPurposeImageReFac&&) = delete;
		GeneralPurposeImageReFac& operator=(GeneralPurposeImageReFac&&) = delete;

		void CopyBufferToImageOnTransferUpload(vk::Buffer buffer_, const std::vector<vk::BufferImageCopy2>& bufferCopyRegions_);
		//void CopyBufferToImage1mip1levelOnTransferUpload(VkBuffer buffer, uint32_t width, uint32_t height);

	private:
		GeneralPurposeImageReFac(GraphicsComponent& _gfx, const VkImage _image, const VkDeviceMemory _image_mem, ImageCIEnhanced vk_image_CI);

	private:
		VkDeviceMemory image_mem{ nullptr };
		VkTimelineSemPoolUnsafe& sem_pool;

	};

}        // namespace Anni
