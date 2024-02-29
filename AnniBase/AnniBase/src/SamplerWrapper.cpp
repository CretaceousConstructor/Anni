#include "SamplerWrapper.h"
namespace Anni
{
	vk::SamplerCreateInfo CI::PopulateTexSamplerCI()
	{
		//typedef struct VkSamplerCreateInfo {
		//    VkStructureType         sType;
		//    const void*             pNext;
		//    VkSamplerCreateFlags    flags;
		//    VkFilter                magFilter;
		//    VkFilter                minFilter;
		//    VkSamplerMipmapMode     mipmapMode;
		//    VkSamplerAddressMode    addressModeU;
		//    VkSamplerAddressMode    addressModeV;
		//    VkSamplerAddressMode    addressModeW;

		//    float                   mipLodBias;
		//    VkBool32                anisotropyEnable;
		//    float                   maxAnisotropy;
		//    VkBool32                compareEnable;
		//    VkCompareOp             compareOp;
		//    float                   minLod;
		//    float                   maxLod;
		//    VkBorderColor           borderColor;
		//    VkBool32                unnormalizedCoordinates;
		//} VkSamplerCreateInfo;

		vk::SamplerCreateInfo sampler_CI{};
		sampler_CI.magFilter = vk::Filter::eLinear;
		sampler_CI.minFilter = vk::Filter::eLinear;;
		sampler_CI.mipmapMode = vk::SamplerMipmapMode::eLinear;

		sampler_CI.addressModeU = vk::SamplerAddressMode::eRepeat;
		sampler_CI.addressModeV = vk::SamplerAddressMode::eRepeat;
		sampler_CI.addressModeW = vk::SamplerAddressMode::eRepeat;

		//VkPhysicalDeviceProperties properties{};
		//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);

		sampler_CI.mipLodBias = 0.f;
		sampler_CI.anisotropyEnable = VK_FALSE;
		//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
		sampler_CI.maxAnisotropy = 0.f;

		sampler_CI.compareEnable = VK_FALSE;


		sampler_CI.compareOp = vk::CompareOp::eAlways;

		sampler_CI.borderColor = vk::BorderColor::eIntOpaqueWhite;
		sampler_CI.unnormalizedCoordinates = VK_FALSE;

		sampler_CI.minLod = 0.0f;
		sampler_CI.maxLod = 1.0f;
		//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
		return sampler_CI;
	}

	vk::SamplerCreateInfo CI::PopulateCubeTexSamplerCI()
	{
		//typedef struct VkSamplerCreateInfo {
		//    VkStructureType         sType;
		//    const void*             pNext;
		//    VkSamplerCreateFlags    flags;
		//    VkFilter                magFilter;
		//    VkFilter                minFilter;
		//    VkSamplerMipmapMode     mipmapMode;
		//    VkSamplerAddressMode    addressModeU;
		//    VkSamplerAddressMode    addressModeV;
		//    VkSamplerAddressMode    addressModeW;

		//    float                   mipLodBias;
		//    VkBool32                anisotropyEnable;
		//    float                   maxAnisotropy;
		//    VkBool32                compareEnable;
		//    VkCompareOp             compareOp;
		//    float                   minLod;
		//    float                   maxLod;
		//    VkBorderColor           borderColor;
		//    VkBool32                unnormalizedCoordinates;
		//} VkSamplerCreateInfo;

		vk::SamplerCreateInfo sampler_CI{};
		sampler_CI.magFilter = vk::Filter::eLinear;;
		sampler_CI.minFilter = vk::Filter::eLinear;;
		sampler_CI.mipmapMode = vk::SamplerMipmapMode::eLinear;;

		sampler_CI.addressModeU = vk::SamplerAddressMode::eClampToEdge;
		sampler_CI.addressModeV = vk::SamplerAddressMode::eClampToEdge;
		sampler_CI.addressModeW = vk::SamplerAddressMode::eClampToEdge;

		sampler_CI.mipLodBias = 0.f;
		//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
		sampler_CI.anisotropyEnable = VK_FALSE;
		sampler_CI.maxAnisotropy = 1.f;

		sampler_CI.compareEnable = VK_FALSE;
		sampler_CI.compareOp = vk::CompareOp::eNever;

		sampler_CI.borderColor = vk::BorderColor::eFloatOpaqueWhite;
		sampler_CI.unnormalizedCoordinates = VK_FALSE;

		sampler_CI.minLod = 0.0f;
		sampler_CI.maxLod = 1.0f;
		//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
		return sampler_CI;
	}




	SamplerWrapper::SamplerWrapper(DeviceManager& device_manager_, VkSamplerCreateInfo sampler_CI_) :
		device_manager(device_manager_), sampler_CI(sampler_CI_)
	{
		sampler = device_manager.GetLogicalDevice().createSampler(sampler_CI);
	}

	vk::Sampler SamplerWrapper::GetRawSampler()
	{
		return sampler;
	}

	SamplerWrapper::~SamplerWrapper()
	{
		device_manager.GetLogicalDevice().destroySampler(sampler);
	}

	vk::WriteDescriptorSet SamplerWrapper::GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement)
	{


		image_info.sampler = sampler;

		vk::WriteDescriptorSet temp_write_descriptor_set{};
		temp_write_descriptor_set.dstBinding = dstbinding;
		temp_write_descriptor_set.descriptorType = vk::DescriptorType::eSampler;
		temp_write_descriptor_set.pImageInfo = &image_info;
		temp_write_descriptor_set.descriptorCount = 1;
		temp_write_descriptor_set.dstArrayElement = dstArrayElement;

		return temp_write_descriptor_set;
	}

	vk::WriteDescriptorSet SamplerWrapper::GetWriteDescriptorSetInfo(vk::DescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement)
	{
		image_info.sampler = sampler;

		vk::WriteDescriptorSet temp_write_descriptor_set{};
		temp_write_descriptor_set.dstBinding = dstbinding;
		temp_write_descriptor_set.dstSet = set;
		temp_write_descriptor_set.descriptorType = vk::DescriptorType::eSampler;
		temp_write_descriptor_set.pImageInfo = &image_info;
		temp_write_descriptor_set.descriptorCount = 1;
		temp_write_descriptor_set.dstArrayElement = dstArrayElement;

		return temp_write_descriptor_set;
	}

}        // namespace Anni
