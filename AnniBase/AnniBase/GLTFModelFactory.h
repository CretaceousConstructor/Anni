#pragma once
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>

#include "BufferFactory.h"
#include "GLTFMaterialInstance.h"
#include "GLTFMetallicRoughness.h"
#include "MeshAsset.h"
#include "Renderable.h"
#include "SamplerWrapper.h"
#include "TextureFactory.h"

namespace Anni
{
	class GLTFModelFactory
	{
	private:
		static vk::Filter ExtractFilter(fastgltf::Filter filter);
		static vk::SamplerMipmapMode ExtractMipmapMode(fastgltf::Filter filter);

	public:
		GLTFModelFactory(DeviceManager& device_manager_,
		                 BufferFactory& buf_fac_,
		                 VkTextureFactory& tex_fac_,
		                 GLTFMetallicRoughnessProducer& mat_metallic_roughness_);

		std::shared_ptr<LoadedGLTF> LoadGLTF(std::string_view gltf_file_path);

		std::shared_ptr<VkTexture> load_image(
			fastgltf::Asset& asset,
			fastgltf::Image& image,
			const std::filesystem::path& gltf_file_path);

		MeshAsset::GPUMeshBuffers upload_mesh(std::span<const uint32_t> indices,
		                                      std::span<const Vertex> vertices);

	private:
		DeviceManager& device_manager;
		BufferFactory& buf_fac;
		VkTextureFactory& tex_fac;
		GLTFMetallicRoughnessProducer& mat_metallic_roughness_producer;
		std::shared_ptr<DescriptorSetAllocatorGrowable> descriptor_allocator;
	};
}
