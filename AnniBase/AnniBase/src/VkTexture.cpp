#include "VkTexture.h"

namespace Anni
{
	VkTexture::VkTexture(
		GraphicsComponent& gfx_, std::optional<std::string> image_path, std::shared_ptr<ImageBaseReFac> image_,
		ImgSyncInfo sync_info_onload_
	) :

		tex_name(std::move(image_path)),
		tex_image(std::move(image_)),
		sync_info_onload(sync_info_onload_),
		gfx(gfx_),
		device_manager(gfx_.DeviceMan())
	{
	}

	void VkTexture::SetImgViewCI(vk::ImageViewCreateInfo image_view_CI_)
	{
		img_view_CI = image_view_CI_;
	}

	void VkTexture::SetSamplerCI(vk::SamplerCreateInfo texture_sampler_CI)
	{
		sampler_CI = texture_sampler_CI;
	}



	vk::WriteDescriptorSet VkTexture::GetWriteDescriptorSetInfo(RenderGraphV1::TexUsage& tex_usage, VkDescriptorSet set)
	{
		if (!tex_usage.img_view_CI)
		{
			tex_usage.image_info.imageView = GetTextureImageView().GetRawImgView();
		}
		else
		{
			tex_usage.img_view = std::make_shared<ImgViewWrapper>(device_manager, tex_usage.img_view_CI.value());
			tex_usage.image_info.imageView = tex_usage.img_view->GetRawImgView();
		}

		if (!tex_usage.sampler_CI)
		{
			tex_usage.image_info.sampler = GetTextureSampler().GetRawSampler();
		}
		else
		{
			tex_usage.sampler = std::make_shared<SamplerWrapper>(device_manager, tex_usage.sampler_CI.value());
			tex_usage.image_info.sampler = tex_usage.sampler->GetRawSampler();
		}
		tex_usage.image_info.imageLayout = tex_usage.sync_info.layout_inpass;



		vk::WriteDescriptorSet temp_write_descriptor_set{};
		temp_write_descriptor_set.dstSet = set;
		temp_write_descriptor_set.dstBinding = tex_usage.desc_info.slot_info.binding;
		temp_write_descriptor_set.dstArrayElement = tex_usage.desc_info.slot_info.array_element;
		temp_write_descriptor_set.descriptorType = tex_usage.desc_info.descriptor_type;

		temp_write_descriptor_set.setImageInfo(tex_usage.image_info);

		return temp_write_descriptor_set;
	}




	//VkWriteDescriptorSet VkTexture::GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement)
	//{
	//	image_info.imageView = GetTextureImageView().GetRawImgView();
	//	image_info.sampler = GetTextureSampler().GetRawSampler();
	//	image_info.imageLayout = current_image_layout;

	//	VkWriteDescriptorSet temp_write_descriptor_set{};
	//	temp_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//	temp_write_descriptor_set.dstBinding = dstbinding;
	//	temp_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	//	temp_write_descriptor_set.dstSet = set;
	//	temp_write_descriptor_set.pImageInfo = &image_info;
	//	temp_write_descriptor_set.descriptorCount = 1;
	//	temp_write_descriptor_set.dstArrayElement = dstArrayElement;

	//	return temp_write_descriptor_set;
	//}

	std::shared_ptr<ImageBaseReFac> VkTexture::GetTextureImage()
	{
		return tex_image;
	}

	VkImage VkTexture::GetTextureRawImage() const
	{
		return tex_image->GetImage();
	}

	ImgViewWrapper& VkTexture::GetTextureImageView() const
	{
		return *tex_image_view;
	}

	SamplerWrapper& VkTexture::GetTextureSampler() const
	{
		return *tex_sampler;
	}




	//void VkTexture::InsertImageMemoryBarrier(VkCommandBuffer                           cmd_buffer,
	//                                         const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier_enhanced)
	//{
	//	VkImageMemoryBarrier image_memory_barrier{};
	//	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//	image_memory_barrier.pNext = nullptr;
	//
	//	image_memory_barrier.srcQueueFamilyIndex = img_mem_barrier_enhanced.srcQueueFamilyIndex;
	//	image_memory_barrier.dstQueueFamilyIndex = img_mem_barrier_enhanced.dstQueueFamilyIndex;
	//
	//	image_memory_barrier.image     = tex_image->GetImage();
	//	image_memory_barrier.newLayout = img_mem_barrier_enhanced.newLayout;
	//	image_memory_barrier.oldLayout = img_mem_barrier_enhanced.oldLayout;
	//
	//	image_memory_barrier.srcAccessMask = img_mem_barrier_enhanced.srcAccessMask;
	//	image_memory_barrier.dstAccessMask = img_mem_barrier_enhanced.dstAccessMask;
	//
	//	//typedef struct VkImageSubresourceRange {
	//	//    VkImageAspectFlags    aspectMask;
	//	//    uint32_t              baseMipLevel;
	//	//    uint32_t              levelCount;
	//	//    uint32_t              baseArrayLayer;
	//	//    uint32_t              layerCount;
	//	//} VkImageSubresourceRange;
	//	const auto& vk_image_CI = tex_image->GetCI();
	//
	//	//SUBRESOURCES RANGES
	//	if (img_mem_barrier_enhanced.subresource_range.has_value())
	//	{
	//		//Determine by user
	//		image_memory_barrier.subresourceRange = img_mem_barrier_enhanced.subresource_range.value();
	//	}
	//	else
	//	{
	//		//Default setting
	//		image_memory_barrier.subresourceRange.baseMipLevel = 0;
	//		image_memory_barrier.subresourceRange.levelCount   = vk_image_CI.default_image_CI.mipLevels;
	//
	//		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	//		image_memory_barrier.subresourceRange.layerCount     = vk_image_CI.default_image_CI.arrayLayers;
	//
	//		//TODO:aspectMask���ж�Ҫ��image_format������image_view_format?��������image_format����Ϊ���߲���Ӧ�ò���̫��ͬʱ������ֻ��ָ��image��ָ��)
	//
	//		const auto img_format = vk_image_CI.default_image_format;
	//
	//		if ((img_format == VK_FORMAT_D32_SFLOAT) || (img_format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (img_format == VK_FORMAT_D24_UNORM_S8_UINT))
	//		{
	//			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	//			if (Vk::HasStencilComponent(vk_image_CI.default_image_format))
	//			{
	//				image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	//			}
	//		}
	//		else
	//		{
	//			//aspectMask must be only VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT or VK_IMAGE_ASPECT_STENCIL_BIT if format is a color, depth-only or stencil-only format, respectively
	//			//If using a depth/stencil format with both depth and stencil components, aspectMask must include at least one of VK_IMAGE_ASPECT_DEPTH_BIT and VK_IMAGE_ASPECT_STENCIL_BIT, and can include both.
	//			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//		}
	//
	//		//if (img_mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	//		//{
	//		//	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	//		//	if (Vk::HasStencilComponent(para_pack->default_image_format))
	//		//	{
	//		//		image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	//		//	}
	//		//}
	//		//else
	//		//{
	//		//	//aspectMask must be only VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT or VK_IMAGE_ASPECT_STENCIL_BIT if format is a color, depth-only or stencil-only format, respectively
	//		//	//If using a depth/stencil format with both depth and stencil components, aspectMask must include at least one of VK_IMAGE_ASPECT_DEPTH_BIT and VK_IMAGE_ASPECT_STENCIL_BIT, and can include both.
	//		//	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//		//}
	//	}
	//
	//	//�����queue ownership transferӦ�����û�����
	//	//*****************************release operation*****************************
	//	//const auto transfer_command_quque = device_manager.GetTransferQueue();
	//	//const auto transfer_command_pool  = gfx.CommandMan().transfer_command_pool;
	//
	//	//const VkCommandBuffer transfer_commandBuffer = VkCommandManager::BeginSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice());
	//
	//	//sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
	//	//barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	//	//barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//	////dstAccessMask is ignored for such a barrier,
	//	////Whilst it is not invalid to provide destination or source access masks for memory
	//	////barriers used for release or acquire operations, respectively, they have no practical effect.
	//	////destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
	//	//destinationStage      = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	//	//barrier.dstAccessMask = 0;
	//	//barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//
	//	//barrier.srcQueueFamilyIndex = queue_family_indices.transfer_family.value();
	//	//barrier.dstQueueFamilyIndex = queue_family_indices.graphics_family.value();
	//
	//	//vkCmdPipelineBarrier(
	//	//    transfer_commandBuffer,
	//	//    sourceStage, destinationStage,
	//	//    0,
	//	//    0, nullptr,
	//	//    0, nullptr,
	//	//    1, &barrier);
	//
	//	////VkCommandManager::EndSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice(), transfer_commandBuffer, transfer_command_quque);
	//	///
	//	///
	//	///
	//	////**************************acquire operation*****************************
	//	//sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	//	//barrier.srcAccessMask = 0;
	//	//barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//	////dstAccessMask is ignored for such a barrier,
	//	////Whilst it is not invalid to provide destination or source access masks for memory
	//	////barriers used for release or acquire operations, respectively, they have no practical
	//	////effect.
	//	//destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//	//barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	//	//barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//
	//	//barrier.srcQueueFamilyIndex = queue_family_indices.transfer_family.value();
	//	//barrier.dstQueueFamilyIndex = queue_family_indices.graphics_family.value();
	//
	//	//vkCmdPipelineBarrier(
	//	//    command_buffer,
	//	//    sourceStage, destinationStage,
	//	//    0,
	//	//    0, nullptr,
	//	//    0, nullptr,
	//	//    1, &barrier);
	//
	//	vkCmdPipelineBarrier(
	//	    cmd_buffer,
	//	    img_mem_barrier_enhanced.srcStageMask,
	//	    img_mem_barrier_enhanced.dstStageMask,
	//	    0,
	//	    0, nullptr,
	//	    0, nullptr,
	//	    1, &image_memory_barrier);
	//
	//	current_image_layout = image_memory_barrier.newLayout;
	//}
	//
	//void VkTexture::InsertImageMemoryBarrier(const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier_enhanced, const DeviceManager::CommandPoolType command_type)
	//{
	//	//VkCommandPool command_pool;
	//	//VkQueue       command_quque;
	//
	//	//const DeviceManager::QueueFamilyIndices queue_family_indices = DeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), gfx.Window().GetSurface());
	//
	//	//if (command_type == DeviceManager::CommandPoolType::graphics_command_pool)
	//	//{
	//	//	command_quque = device_manager.GetGraphicsQueue();
	//	//	command_pool  = gfx.CommandMan().graphics_command_pool;
	//	//}
	//
	//	//else if (command_type == DeviceManager::CommandPoolType::transfor_command_pool)
	//	//{
	//	//	command_quque = device_manager.GetTransferQueue();
	//	//	command_pool  = gfx.CommandMan().transfer_command_pool;
	//	//}
	//	//else
	//	//{
	//	//	throw std::runtime_error("unknown type of command buffer.");
	//	//}
	//
	//	//const VkCommandBuffer command_buffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());
	//
	//	//VkImageMemoryBarrier barrier{};
	//	//barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//	//barrier.pNext = nullptr;
	//
	//	////TODO: srcQueueFamilyIndex and dstQueueFamilyIndex
	//	//barrier.srcQueueFamilyIndex = img_mem_barrier_enhanced.srcQueueFamilyIndex;
	//	//barrier.dstQueueFamilyIndex = img_mem_barrier_enhanced.dstQueueFamilyIndex;
	//
	//	//barrier.oldLayout = img_mem_barrier_enhanced.oldLayout;
	//	//barrier.newLayout = img_mem_barrier_enhanced.newLayout;
	//
	//	//barrier.srcAccessMask = img_mem_barrier_enhanced.srcAccessMask;
	//	//barrier.dstAccessMask = img_mem_barrier_enhanced.dstAccessMask;
	//
	//	//barrier.image              = tex_image->GetImage();
	//	//const auto image_para_pack = tex_image->GetCI();
	//
	//	//if (img_mem_barrier_enhanced.subresource_range.has_value())
	//	//{
	//	//	barrier.subresourceRange = img_mem_barrier_enhanced.subresource_range.value();
	//	//}
	//	//else
	//	//{
	//	//	barrier.subresourceRange.baseMipLevel = 0;
	//	//	barrier.subresourceRange.levelCount   = image_para_pack->default_image_CI.mipLevels;
	//
	//	//	barrier.subresourceRange.baseArrayLayer = 0;
	//	//	barrier.subresourceRange.layerCount     = image_para_pack->default_image_CI.arrayLayers;
	//
	//	//	const auto img_format = image_para_pack->default_image_format;
	//
	//	//	if ((img_format == VK_FORMAT_D32_SFLOAT) || (img_format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (img_format == VK_FORMAT_D24_UNORM_S8_UINT))
	//	//	{
	//	//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	//	//		if (Vk::HasStencilComponent(image_para_pack->default_image_format))
	//	//		{
	//	//			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	//	//		}
	//	//	}
	//	//	else
	//	//	{
	//	//		//aspectMask must be only VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT or VK_IMAGE_ASPECT_STENCIL_BIT if format is a color, depth-only or stencil-only format, respectively
	//	//		//If using a depth/stencil format with both depth and stencil components, aspectMask must include at least one of VK_IMAGE_ASPECT_DEPTH_BIT and VK_IMAGE_ASPECT_STENCIL_BIT, and can include both.
	//	//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//	//	}
	//
	//	//	//if (img_mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	//	//	//{
	//	//	//	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	//
	//	//	//	if (Vk::HasStencilComponent(para_pack->default_image_format))
	//	//	//	{
	//	//	//		barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	//	//	//	}
	//	//	//}
	//	//	//else
	//	//	//{
	//	//	//	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//	//	//}
	//	//}
	//
	//	//VkPipelineStageFlags sourceStage      = img_mem_barrier_enhanced.srcStageMask;
	//	//VkPipelineStageFlags destinationStage = img_mem_barrier_enhanced.dstStageMask;
	//
	//	//////check situations where queue ownership transfer may happend.
	//	////if (
	//	////    (img_mem_barrier_enhanced.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && img_mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) &&
	//	////    (VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.srcQueueFamilyIndex || VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.dstQueueFamilyIndex))
	//	////{
	//	////	throw std::runtime_error("queue owner ship transfer operation may be required, but queue families are not specified.");
	//	////}
	//
	//	//if (VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.srcQueueFamilyIndex && VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.dstQueueFamilyIndex)
	//	//{
	//	//	vkCmdPipelineBarrier(
	//	//	    command_buffer,
	//	//	    sourceStage, destinationStage,
	//	//	    0,
	//	//	    0, nullptr,
	//	//	    0, nullptr,
	//	//	    1, &barrier);
	//
	//	//	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), command_buffer, command_quque);
	//	//}
	//	////*****************************ownership transfer operation*****************************
	//	//else if (img_mem_barrier_enhanced.srcQueueFamilyIndex != img_mem_barrier_enhanced.dstQueueFamilyIndex)
	//	//{
	//	//	//*****************************ownership release operation*****************************
	//	//	const auto transfer_command_quque = device_manager.GetTransferQueue();
	//	//	const auto transfer_command_pool  = gfx.CommandMan().transfer_command_pool;
	//
	//	//	const VkCommandBuffer transfer_commandBuffer = VkCommandManager::BeginSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice());
	//
	//	//	sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
	//	//	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	//	//	barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//	//	//dstAccessMask is ignored for such a barrier,
	//	//	//Whilst it is not invalid to provide destination or source access masks for memory
	//	//	//barriers used for release or acquire operations, respectively, they have no practical
	//	//	//effect.
	//	//	//destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
	//	//	destinationStage      = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	//	//	barrier.dstAccessMask = VK_ACCESS_NONE;
	//	//	barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//
	//	//	barrier.srcQueueFamilyIndex = queue_family_indices.transfer_family.value();
	//	//	barrier.dstQueueFamilyIndex = queue_family_indices.graphics_family.value();
	//
	//	//	vkCmdPipelineBarrier(
	//	//	    transfer_commandBuffer,
	//	//	    sourceStage, destinationStage,
	//	//	    0,
	//	//	    0, nullptr,
	//	//	    0, nullptr,
	//	//	    1, &barrier);
	//
	//	//	VkCommandManager::EndSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice(), transfer_commandBuffer, transfer_command_quque);
	//	//	//**************************ownership acquire operation*****************************
	//	//	sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	//	//	barrier.srcAccessMask = VK_ACCESS_NONE;
	//	//	barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//	//	//dstAccessMask is ignored for such a barrier,
	//	//	//Whilst it is not invalid to provide destination or source access masks for memory
	//	//	//barriers used for release or acquire operations, respectively, they have no practical
	//	//	//effect.
	//	//	destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//	//	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	//	//	barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//
	//	//	barrier.srcQueueFamilyIndex = queue_family_indices.transfer_family.value();
	//	//	barrier.dstQueueFamilyIndex = queue_family_indices.graphics_family.value();
	//
	//	//	vkCmdPipelineBarrier(
	//	//	    command_buffer,
	//	//	    sourceStage, destinationStage,
	//	//	    0,
	//	//	    0, nullptr,
	//	//	    0, nullptr,
	//	//	    1, &barrier);
	//
	//	//	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), command_buffer, command_quque);
	//	//}
	//	//else if (img_mem_barrier_enhanced.srcQueueFamilyIndex == img_mem_barrier_enhanced.dstQueueFamilyIndex)
	//	//{
	//	//	//TODO:
	//	//}
	//	//else
	//	//{
	//	//}
	//
	//	//current_image_layout = img_mem_barrier_enhanced.newLayout;
	//}

	[[nodiscard]] std::string VkTexture::GetTextureName() const
	{
		assert(tex_name);
		return tex_name.value();
	}

	void VkTexture::SetCISamplerAndImgView(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_)
	{
		sampler_CI = sampler_CI_;
		img_view_CI = img_view_CI_;
	}
}        // namespace Anni
