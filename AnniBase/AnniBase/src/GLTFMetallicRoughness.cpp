#include "GLTFMetallicRoughness.h"

Anni::GLTFMetallicRoughnessProducer::GLTFMetallicRoughnessProducer(DeviceManager& device_manager_,
	DescriptorLayoutManager& descriptor_layout_manager_, PipelineBuilder& pipeline_builder_):
	device_manager(device_manager_),
	descriptor_layout_manager(descriptor_layout_manager_),
	pipeline_builder(pipeline_builder_)
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

	materialLayout = descriptor_layout_manager.GetMatDescriptorSetLayout<decltype(*this)>(set_layout_CI);     //per-material 我们给每一个材质单独的set = 1
	matrixRange.offset = 0;
	matrixRange.size = sizeof(GPUDrawPushConstants);
	matrixRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
}

std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> Anni::GLTFMetallicRoughnessProducer::BuildPipelines(
	GFXPipelineCI gfx_pip_CI)
{
	//DESCRIPTOR SET LAYOUTS
	gfx_pip_CI.AddPushConstantRange(matrixRange);
	gfx_pip_CI.AddDescriptorSetLayout(materialLayout);

	//opaquePipeline.pipeline = std::move(pipline_and_pipeline_layout.first);
	//opaquePipeline.pipeline_layout = std::move(pipline_and_pipeline_layout.second);
	return  pipeline_builder.BuildPipeline(gfx_pip_CI);
}

Anni::MaterialInstance Anni::GLTFMetallicRoughnessProducer::Instantiate(MaterialPassType pass_t,
                                                                        DescriptorSetAllocatorGrowable& descriptorAllocator)
{
	MaterialInstance matData;
	matData.passType = pass_t;
	matData.materialSet = descriptorAllocator.Allocate(materialLayout);
	return matData;
}
