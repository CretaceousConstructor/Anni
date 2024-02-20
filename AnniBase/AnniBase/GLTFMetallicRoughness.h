#pragma once
#include "Buffer.h"
#include "VkTexture.h"
#include "DesSetLayoutManager.h"
#include "VkPipelineBuilder.h"
#include "DescriptorAllocator.h"
#include "Mate.h"

namespace Anni
{

	//�ṩpipeline����������䶯���� �ṩdescriptor set layout��������ǰ������
	//��������ΪGLTFMetallicRoughnessProducer

	//TODO: Pipeline pre-build

	class GLTFMetallicRoughnessProducer
	{
	public:
		GLTFMetallicRoughnessProducer(
			DeviceManager& device_manager_,
			DescriptorLayoutManager& descriptor_layout_manager_,
			VkPipelineBuilder& pipeline_builder_
		) :
			device_manager(device_manager_),
			descriptor_layout_manager(descriptor_layout_manager_),
			pipeline_builder(pipeline_builder)
		{
			std::vector<vk::DescriptorSetLayoutBinding> bindings;
			bindings.reserve(6);

			//Not necessary for every instance, but we don't want to deal with static init shit.
			//0 : data buf
			bindings.emplace_back(Vk::Binding<0>, vk::DescriptorType::eUniformBuffer, Vk::BindingCount<1>, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex);

			//1 : albedo
			bindings.emplace_back(Vk::Binding<1>, vk::DescriptorType::eCombinedImageSampler, Vk::BindingCount<1>, vk::ShaderStageFlagBits::eFragment);

			//2 : metal roughness
			bindings.emplace_back(Vk::Binding<2>, vk::DescriptorType::eCombinedImageSampler, Vk::BindingCount<1>, vk::ShaderStageFlagBits::eFragment);

			//3 : normal
			bindings.emplace_back(Vk::Binding<3>, vk::DescriptorType::eCombinedImageSampler, Vk::BindingCount<1>, vk::ShaderStageFlagBits::eFragment);

			//4 : emissive
			bindings.emplace_back(Vk::Binding<4>, vk::DescriptorType::eCombinedImageSampler, Vk::BindingCount<1>, vk::ShaderStageFlagBits::eFragment);

			//5 : occlusion
			bindings.emplace_back(Vk::Binding<5>, vk::DescriptorType::eCombinedImageSampler, Vk::BindingCount<1>, vk::ShaderStageFlagBits::eFragment);



			vk::DescriptorSetLayoutCreateInfo set_layout_CI{};
			set_layout_CI.setBindings(bindings);

			materialLayout = descriptor_layout_manager.GetMatDescriptorSetLayout<decltype(*this)>(set_layout_CI);     //per-material ���Ǹ�ÿһ�����ʵ�����set = 1
			matrixRange.offset = 0;
			matrixRange.size = sizeof(GPUDrawPushConstants);
			matrixRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
		}


	private:
		DeviceManager& device_manager;
		DescriptorLayoutManager& descriptor_layout_manager;
		VkPipelineBuilder& pipeline_builder;
	private:
		//MaterialPipeline transparentPipeline;
		//MaterialPipeline        opaquePipeline;

		vk::DescriptorSetLayout materialLayout;
		vk::PushConstantRange   matrixRange;
	public:
		std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> BuildPipelines(GFXPipelineCI gfx_pip_CI)
		{
			//DESCRIPTOR SET LAYOUTS
			gfx_pip_CI.AddPushConstantRange(matrixRange);
			gfx_pip_CI.AddDescriptorSetLayout(materialLayout);

			//opaquePipeline.pipeline = std::move(pipline_and_pipeline_layout.first);
			//opaquePipeline.pipeline_layout = std::move(pipline_and_pipeline_layout.second);
			return  pipeline_builder.BuildPipeline(gfx_pip_CI);
		}

		MaterialInstance Instantiate(MaterialPassType pass_t, DescriptorSetAllocatorGrowable& descriptorAllocator)
		{
			MaterialInstance matData;
			matData.passType = pass_t;
			matData.materialSet = descriptorAllocator.Allocate(materialLayout);
			return matData;
		}


	};

}

