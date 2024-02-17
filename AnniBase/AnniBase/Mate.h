#pragma once
#include "AnniMath.h"
#include "Buffer.h"
#include "VkTexture.h"
#include "VkShaderWrapper.h"
#include "VkDescriptorManagerV0.h"
#include "DesSetLayoutManager.h"
#include "VkPipelineBuilder.h"
#include "GFXPipelineCIBuilder.h"
#include "DescriptorAllocator.h"

namespace Anni
{


	enum class MaterialPassType :uint8_t
	{
		MainColor,
		Transparent,
		Other
	};

	struct GPUDrawPushConstants
	{
		glm::mat4 worldMatrix;
		VkDeviceAddress vertexBuffer;
	};



	struct MaterialPipeline
	{
		MaterialPipeline() = default;
			
		~MaterialPipeline() = default;

		MaterialPipeline(const MaterialPipeline&) = delete;
		MaterialPipeline& operator=(const MaterialPipeline&) = delete;

		MaterialPipeline(MaterialPipeline&&) = delete;
		MaterialPipeline& operator=(MaterialPipeline&&) = delete;


		std::unique_ptr<PipelineWrapper> pipeline;
		std::unique_ptr<PipelineLayoutWrapper> pipeline_layout;

	};

	struct MaterialInstance   //材质实例，其中pipeline和pipelinelayout是从观察指针pipeline得到的
	{
		MaterialPipeline* pipeline;
		VkDescriptorSet materialSet;
		MaterialPassType passType;
	};


	struct GLTFMaterial
	{
		MaterialInstance data;
	};



	//提供pipeline（根据需求变动）， 提供descriptor set layout（可以提前构建）
	class GLTFMetallicRoughness
	{
	public:

		struct MaterialConstants
		{
			glm::vec4 colorFactors;
			glm::vec4 metal_rough_factors;
			//padding, we need it anyway for uniform buffers
			glm::vec4 extra[14];
		};

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


		GLTFMetallicRoughness(DeviceManager& device_manager_, DescriptorLayoutManager& descriptor_manager_, VkPipelineBuilder& pipeline_builder_, GFXPipelineCIBuilder& gfx_pipelineCI_builder_) :
			device_manager(device_manager_),
			descriptor_manager(descriptor_manager_),
			pipeline_builder(pipeline_builder),
			gfx_pipelineCI_builder(gfx_pipelineCI_builder_)
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			bindings.reserve(6);
			//Not necessary for every instance, but we don't want to deal with static init shit.
			//0 : data buf
			bindings.push_back(DescVk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT));

			//1 : albedo
			bindings.push_back(DescVk::GetDescriptorSetLayoutBinding(Vk::Binding<1>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT));

			//2 : metal roughness
			bindings.push_back(DescVk::GetDescriptorSetLayoutBinding(Vk::Binding<2>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT));

			//3 : normal
			bindings.push_back(DescVk::GetDescriptorSetLayoutBinding(Vk::Binding<3>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT));

			//4 : emissive
			bindings.push_back(DescVk::GetDescriptorSetLayoutBinding(Vk::Binding<4>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT));

			//5 : occlusion
			bindings.push_back(DescVk::GetDescriptorSetLayoutBinding(Vk::Binding<5>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT));

			//materialLayout = descriptor_manager.GetMatDescriptorSetLayout<GLTFMetallicRoughness>(bindings);
			materialLayout = descriptor_manager.GetMatDescriptorSetLayout<decltype(*this)>(bindings);     //per-material 我们给每一个材质单独的set = 1

			matrixRange.offset = 0;
			matrixRange.size = sizeof(GPUDrawPushConstants);
			matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		}


	private:
		DeviceManager& device_manager;
		DescriptorLayoutManager& descriptor_manager;
		VkPipelineBuilder& pipeline_builder;
		GFXPipelineCIBuilder& gfx_pipelineCI_builder;
	private:
		//MaterialPipeline transparentPipeline;
		MaterialPipeline opaquePipeline;
		std::shared_ptr<DescriptorSetLayoutWrapper> materialLayout;
		VkPushConstantRange matrixRange;
	public:
		void BuildPipelines(GFXPipelineCI gfx_pip_CI)
		{
			//DESCRIPTOR SET LAYOUTS
			gfx_pip_CI.AddPushConstantRange(matrixRange);
			gfx_pip_CI.AddDescriptorSetLayout(materialLayout->GetRawDescriptorSetLayout());

			std::pair<std::unique_ptr<PipelineWrapper>, std::unique_ptr<PipelineLayoutWrapper>> pipline_and_pipeline_layout = pipeline_builder.BuildPipeline(gfx_pip_CI);
			opaquePipeline.pipeline        =    std::move(pipline_and_pipeline_layout.first);
			opaquePipeline.pipeline_layout = std::move(pipline_and_pipeline_layout.second);
		}


	public:
		MaterialInstance WriteMaterial(MaterialPassType pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator)
		{
			MaterialInstance matData;
			matData.passType = pass;
			matData.pipeline = &opaquePipeline;
			matData.materialSet = descriptorAllocator.Allocate(materialLayout->GetRawDescriptorSetLayout());

			std::vector<VkWriteDescriptorSet> write_descriptor_sets;
			write_descriptor_sets.reserve(6);

			write_descriptor_sets.push_back(resources.dataBuffer->GetWriteDescriptorSetInfo(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::BindingArrayElement<0>, sizeof(MaterialConstants), resources.dataBufferOffset));
			write_descriptor_sets.push_back(resources.albedoImage->GetWriteDescriptorSetInfo(matData.materialSet, Vk::Binding<1>));
			const auto binding2 = resources.metalRoughImage->GetWriteDescriptorSetInfo(matData.materialSet, Vk::Binding<2>);
			const auto binding3 = resources.normalImage->GetWriteDescriptorSetInfo(matData.materialSet, Vk::Binding<3>);
			const auto binding4 = resources.emissiveImage->GetWriteDescriptorSetInfo(matData.materialSet, Vk::Binding<4>);
			const auto binding5 = resources.occlusionImage->GetWriteDescriptorSetInfo(matData.materialSet, Vk::Binding<5>);

			//UPDATE DESCRIPTOR SET
			vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);

			// UPDATE DESCRIPTOR SET
			return matData;
		}

	};






}