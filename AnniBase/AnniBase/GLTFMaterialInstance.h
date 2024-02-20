#pragma once


#include "Mate.h"
#include "Buffer.h"
#include "VkTexture.h"


namespace Anni
{

	struct GLTFMaterialInstance
	{
		struct MaterialResources
		{
			std::shared_ptr<Buffer> dataBuffer;

			std::shared_ptr<VkTexture> albedoImage;
			std::shared_ptr<VkTexture> metalRoughImage;
			std::shared_ptr<VkTexture> normalImage;
			std::shared_ptr<VkTexture> emissiveImage;
			std::shared_ptr<VkTexture> occlusionImage;

			uint32_t dataBufferOffset{};
		};

		struct MaterialConstants
		{
			glm::vec4 colorFactors;
			glm::vec4 metal_rough_factors;
			//padding, we need it anyway for uniform buffers
			glm::vec4 extra[14];
		};


		MaterialInstance data;
		MaterialResources materialResources;

		void UpdateDescriptorSet(DeviceManager& device_manager, const RenderGraphV1::TexUsage& tex_usage)
		{
			std::vector<vk::WriteDescriptorSet> write_descriptor_sets;
			write_descriptor_sets.reserve(6);
			{
				vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.dataBuffer->GetWriteDescriptorSetInfo(vk::DescriptorType::eUniformBuffer, Vk::Binding<0>, Vk::BindingArrayElement<0>, sizeof(MaterialConstants), materialResources.dataBufferOffset);

				write_descriptor_sets.push_back(temp_write_descriptor_set);
			}

			{
				vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.albedoImage->GetWriteDescriptorSetInfoForModel(tex_usage);
				temp_write_descriptor_set.setDstSet(data.materialSet);
				temp_write_descriptor_set.setDstBinding(Vk::Binding<1>);
				temp_write_descriptor_set.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
				write_descriptor_sets.push_back(temp_write_descriptor_set);
			}

			{
				vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.metalRoughImage->GetWriteDescriptorSetInfoForModel(tex_usage);
				temp_write_descriptor_set.setDstSet(data.materialSet);
				temp_write_descriptor_set.setDstBinding(Vk::Binding<2>);
				temp_write_descriptor_set.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
				write_descriptor_sets.push_back(temp_write_descriptor_set);
			}

			{
				vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.normalImage->GetWriteDescriptorSetInfoForModel(tex_usage);
				temp_write_descriptor_set.setDstSet(data.materialSet);
				temp_write_descriptor_set.setDstBinding(Vk::Binding<3>);
				temp_write_descriptor_set.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
				write_descriptor_sets.push_back(temp_write_descriptor_set);
			}

			{
				vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.emissiveImage->GetWriteDescriptorSetInfoForModel(tex_usage);
				temp_write_descriptor_set.setDstSet(data.materialSet);
				temp_write_descriptor_set.setDstBinding(Vk::Binding<4>);
				temp_write_descriptor_set.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
				write_descriptor_sets.push_back(temp_write_descriptor_set);
			}


			{
				vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.occlusionImage->GetWriteDescriptorSetInfoForModel(tex_usage);
				temp_write_descriptor_set.setDstSet(data.materialSet);
				temp_write_descriptor_set.setDstBinding(Vk::Binding<5>);
				temp_write_descriptor_set.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
				write_descriptor_sets.push_back(temp_write_descriptor_set);
			}

			device_manager.GetLogicalDevice().updateDescriptorSets(write_descriptor_sets, {});
		}

	};
}

