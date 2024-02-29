#pragma once

#include "DeviceManager.h"

namespace Anni
{
	namespace CI
	{
		[[nodiscard]] vk::SamplerCreateInfo PopulateTexSamplerCI();
		[[nodiscard]] vk::SamplerCreateInfo PopulateCubeTexSamplerCI();
	}        // namespace CI


	class SamplerWrapper
	{
	public:
		SamplerWrapper(DeviceManager& device_manager_, VkSamplerCreateInfo sampler_CI_);
		SamplerWrapper() = delete;

		vk::Sampler GetRawSampler();
		~SamplerWrapper();

		SamplerWrapper(const SamplerWrapper&) = delete;
		SamplerWrapper& operator=(const SamplerWrapper&) = delete;

		SamplerWrapper(SamplerWrapper&& rhs) = delete;
		SamplerWrapper& operator=(SamplerWrapper&&) = delete;


		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0);
		[[nodiscard]] vk::WriteDescriptorSet GetWriteDescriptorSetInfo(vk::DescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0);


	private:
		vk::DescriptorImageInfo image_info{};
	private:
		DeviceManager& device_manager;
		vk::Sampler     sampler;
		vk::SamplerCreateInfo sampler_CI;
	};

}        // namespace Anni
