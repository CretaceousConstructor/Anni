#include "GLTFMaterialInstance.h"

void Anni::GLTFMaterialInstance::UpdateDescriptorSet(DeviceManager& device_manager,
	const RenderGraphV1::TexUsage& tex_usage)
{
	std::vector<vk::WriteDescriptorSet> write_descriptor_sets;
	write_descriptor_sets.reserve(6);
	{
		const vk::WriteDescriptorSet temp_write_descriptor_set = materialResources.dataBuffer->GetWriteDescriptorSetInfo(vk::DescriptorType::eUniformBuffer, Vk::Binding<0>, Vk::BindingArrayElement<0>, sizeof(MaterialConstants), materialResources.dataBufferOffset);

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
