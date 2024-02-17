#include "VkModelFactory.h"

namespace Anni
{
	//VkModelFactory::VkModelFactory(GraphicsComponent& gfx_, VkTextureFactory& texture_factory_, BufferFactory& buffer_factory_) :
	//	gfx(gfx_),
	//	device_manager(gfx.DeviceMan()),
	//	window(gfx.Window()),
	//	texture_factory(texture_factory_),
	//	buffer_factory(buffer_factory_)

	//{
	//}

	//VkModelFactory::GltfModelCI::GltfModelCI(std::string path_, const tinygltf::Model& input_, const Vk::ModelLoadingOption option_) :
	//	path(std::move(path_)),
	//	input(input_),
	//	option(option_)
	//{
	//	const size_t pos = path.find_last_of('/');
	//	model_directory = path.substr(0, pos);
	//}

	//bool VkModelFactory::LoadglTFFile(const std::string& path, tinygltf::Model& glTFInput)
	//{
	//	tinygltf::TinyGLTF gltfContext;
	//	std::string        error, warning;
	//	const bool         file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, path);


	//	std::cout << warning << std::endl;
	//	if (!file_loaded)
	//	{
	//		std::cout << error << std::endl;
	//	}

	//	return file_loaded;
	//}

	//void VkModelFactory::LoadTextures(GltfModelCI& model) const
	//{
	//	auto& textures = model.textures;
	//	auto& image_formats = model.image_formats;
	//	const auto& input = model.input;

	//	std::vector<VkFormat> temp_image_formats;
	//	temp_image_formats.resize(input.images.size(), VK_FORMAT_R8G8B8A8_SRGB);

	//	textures.resize(input.textures.size());
	//	//��texture��Ӧ��image formatת����image��Ӧ��image format
	//	for (size_t i = 0; i < input.textures.size(); i++)
	//	{
	//		textures[i].image_idx = input.textures[i].source;
	//		temp_image_formats[textures[i].image_idx] = image_formats[i];
	//	}

	//	image_formats.swap(temp_image_formats);
	//}

	//void VkModelFactory::LoadImages(GltfModelCI& model) const
	//{
	//	auto& images = model.images;
	//	auto& image_formats = model.image_formats;
	//	const auto& input = model.input;

	//	images.resize(input.images.size());

	//	for (size_t i = 0; i < input.images.size(); i++)
	//	{
	//		auto& format_of_image = image_formats[i];

	//		const tinygltf::Image& glTFImage = input.images[i];

	//		//"../../data/models/FlightHelmet/FlightHelmet.gltf"
	//		const std::string path = model.model_directory + "/" + glTFImage.uri;
	//		const size_t      pos = glTFImage.uri.find_last_of('.');
	//		auto              file_extension = glTFImage.uri.substr(pos + 1, glTFImage.uri.size());


	//		//tinygltf��������Ǽ���ktx�ļ���ʽ��������ʽ�п��ܼ��ء�
	//		if ("ktx" == file_extension || (model.option & Vk::ModelLoadingOption::LoadingImgByOurselves) || !TestIfGltfHadLoadedImg(glTFImage))
	//		{
	//			auto sampler_CI = CI::PopulateTexSamplerCI();
	//			auto img_view_CI = CI::PopulateTextureImgViewCI(format_of_image);
	//			if (stbi_is_hdr(path.c_str()))
	//			{
	//				format_of_image = VK_FORMAT_R16G16B16A16_SFLOAT;
	//			}
	//			images[i] = texture_factory.ProduceTextureFromImgPath(path, format_of_image, sampler_CI, img_view_CI, VK_IMAGE_ASPECT_COLOR_BIT);
	//		}
	//		// ��tinygltf���صĸ�ʽ��Load texture from given gltf image buffer
	//		else
	//		{
	//			// Get the image data from the glTF loader
	//			unsigned char* buffer = nullptr;
	//			VkDeviceSize   bufferSize = 0;
	//			bool           delete_buffer = false;
	//			// If it's an RGB image (3 components), we convert it to RGBA as most Vulkan implementations don't support 8-Bit RGB-only formats
	//			if (glTFImage.component == 3)
	//			{
	//				bufferSize = glTFImage.width * glTFImage.height * 4;
	//				buffer = new unsigned char[bufferSize];
	//				unsigned char* rgba = buffer;
	//				//unsigned char* rgb = &glTFImage.image[0];
	//				const unsigned char* rgb = glTFImage.image.data();
	//				for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i)
	//				{
	//					memcpy(rgba, rgb, sizeof(unsigned char) * 3);
	//					rgba[3] = 0xff;        //Ĭ��AΪ1.0
	//					rgba += 4;
	//					rgb += 3;
	//				}
	//				delete_buffer = true;
	//			}
	//			else
	//			{
	//				buffer = const_cast<unsigned char*>(glTFImage.image.data());
	//				bufferSize = glTFImage.image.size();
	//			}

	//			assert(glTFImage.width > 0);
	//			assert(glTFImage.height > 0);

	//			auto         sampler_CI = CI::PopulateTexSamplerCI();
	//			auto         img_view_CI = CI::PopulateTextureImgViewCI(format_of_image);
	//			ImageCIEnhanced tex_img_CI = CI::GetTextureImgCI(format_of_image, VkExtent3D(uint32_t(glTFImage.width), uint32_t(glTFImage.height), 1), path);
	//			images[i] = texture_factory.ProduceTextureFromBufferReFac(tex_img_CI, sampler_CI, img_view_CI, buffer, VK_IMAGE_ASPECT_COLOR_BIT);

	//			if (delete_buffer)
	//			{
	//				delete[] buffer;
	//			}
	//		}
	//	}
	//}

	//void VkModelFactory::LoadNode(const size_t current_node_index_in_glft, const int parent, GltfModelCI& model) const
	//{
	//	//NODE TREE
	//	const auto& input = model.input;

	//	auto& index_buffer_cpu = model.index_buffer_cpu;
	//	auto& vertex_buffer_cpu = model.vertex_buffer_cpu;

	//	auto& nodes = model.nodes;
	//	const tinygltf::Node& current_node_in_gltf = input.nodes[current_node_index_in_glft];
	//	auto                  index_type = model.index_type_of_cpu_buffer;

	//	assert(model.index_type_of_cpu_buffer == VK_INDEX_TYPE_UINT32);
	//	///BUGS:vector push back results in memory address changed!! solved

	//	nodes.emplace_back();
	//	int current_node_index = static_cast<int>(nodes.size() - 1);

	//	glm::mat4 node_matrix;
	//	node_matrix = glm::mat4(1.0f);

	//	// Get the local node matrix
	//	// It's either made up from translation, rotation, scale or a 4x4 matrix

	//	//Final matrix has the form of                translation * rotation * scale
	//	if (current_node_in_gltf.translation.size() == 3)
	//	{
	//		node_matrix = glm::translate(node_matrix, glm::vec3(glm::make_vec3(current_node_in_gltf.translation.data())));
	//	}
	//	if (current_node_in_gltf.rotation.size() == 4)
	//	{
	//		glm::quat q = glm::make_quat(current_node_in_gltf.rotation.data());
	//		node_matrix *= glm::mat4(q);
	//	}
	//	if (current_node_in_gltf.scale.size() == 3)
	//	{
	//		//When using glm::scale( X, vec3 ), you are multiplying
	//		//X* glm::scale(Identity, vec3)

	//		node_matrix = glm::scale(node_matrix, glm::vec3(glm::make_vec3(current_node_in_gltf.scale.data())));
	//	}
	//	if (current_node_in_gltf.matrix.size() == 16)
	//	{
	//		node_matrix = glm::make_mat4x4(current_node_in_gltf.matrix.data());
	//	};

	//	//Gltf::Node &current_node = nodes[current_node_index];
	//	nodes[current_node_index].matrix = node_matrix;

	//	// Load node's children
	//	if (!current_node_in_gltf.children.empty())
	//	{
	//		for (size_t i = 0; i < current_node_in_gltf.children.size(); i++)
	//		{
	//			LoadNode(current_node_in_gltf.children[i], current_node_index, model);
	//		}
	//	}

	//	// If the node contains mesh data, we load vertices and indices from the buffers
	//	// In glTF this is done via accessors and buffer views
	//	if (current_node_in_gltf.mesh > -1)
	//	{
	//		const tinygltf::Mesh mesh = input.meshes[current_node_in_gltf.mesh];
	//		// Iterate through all primitives of this node's mesh
	//		for (size_t i = 0; i < mesh.primitives.size(); i++)
	//		{
	//			const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];

	//			uint32_t fir_idx = static_cast<uint32_t>(index_buffer_cpu.size());
	//			uint32_t vertexStart = static_cast<uint32_t>(vertex_buffer_cpu.size());
	//			uint32_t idx_count = 0;
	//			uint32_t vertexCount = 0;

	//			// Vertices
	//			{
	//				const float* positionBuffer = nullptr;
	//				const float* normalsBuffer = nullptr;
	//				const float* texCoordsBuffer = nullptr;
	//				const float* tangentsBuffer = nullptr;

	//				// Get buffer data for vertex position
	//				if (glTFPrimitive.attributes.contains("POSITION"))
	//				{
	//					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
	//					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
	//					positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	//					vertexCount = static_cast<uint32_t>(accessor.count);
	//				}
	//				// Get buffer data for vertex normals
	//				if (glTFPrimitive.attributes.contains("NORMAL"))
	//				{
	//					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
	//					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
	//					normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	//				}
	//				// Get buffer data for vertex texture coordinates
	//				// glTF supports multiple sets, we only load the first one
	//				if (glTFPrimitive.attributes.contains("TEXCOORD_0"))
	//				{
	//					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
	//					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
	//					texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	//				}

	//				// Get buffer data for vertex tangent
	//				if (glTFPrimitive.attributes.contains("TANGENT"))
	//				{
	//					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
	//					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
	//					tangentsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
	//				}

	//				// Append data to model's vertex buffer
	//				for (size_t v = 0; v < vertexCount; v++)
	//				{
	//					Gltf::Vertex vert{};
	//					vert.tangent = tangentsBuffer ? glm::make_vec4(&tangentsBuffer[v * 4]) : glm::vec4(0.0f);
	//					vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
	//					vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
	//					vert.color = glm::vec3(1.0f);
	//					vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
	//					//vert.color = model.materials[glTFPrimitive.material].baseColorFactor;
	//					vertex_buffer_cpu.push_back(vert);
	//				}
	//			}
	//			// Indices
	//			{
	//				const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
	//				const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
	//				const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

	//				idx_count += static_cast<uint32_t>(accessor.count);

	//				// glTF supports different component types of indices

	//				switch (accessor.componentType)
	//				{
	//				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
	//				{
	//					index_type = VK_INDEX_TYPE_UINT32;
	//					uint32_t* buf = new uint32_t[accessor.count];
	//					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
	//					for (size_t index = 0; index < accessor.count; index++)
	//					{
	//						index_buffer_cpu.push_back(buf[index] + vertexStart);
	//					}
	//					break;
	//				}
	//				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
	//				{
	//					index_type = VK_INDEX_TYPE_UINT16;

	//					uint16_t* buf = new uint16_t[accessor.count];
	//					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
	//					for (size_t index = 0; index < accessor.count; index++)
	//					{
	//						index_buffer_cpu.push_back(buf[index] + vertexStart);
	//					}
	//					break;
	//				}
	//				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
	//				{
	//					index_type = VK_INDEX_TYPE_UINT8_EXT;
	//					uint8_t* buf = new uint8_t[accessor.count];
	//					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
	//					for (size_t index = 0; index < accessor.count; index++)
	//					{
	//						index_buffer_cpu.push_back(buf[index] + vertexStart);
	//					}
	//					break;
	//				}
	//				default:
	//					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
	//					assert(false);
	//					return;
	//				}
	//			}
	//			Gltf::Primitive primitive{};
	//			primitive.fir_idx = fir_idx;
	//			primitive.idx_count = idx_count;
	//			primitive.material_idx = glTFPrimitive.material;
	//			nodes[current_node_index].mesh.primitives.push_back(primitive);
	//		}
	//	}

	//	if (parent >= 0)
	//	{
	//		nodes[current_node_index].parent_index = parent;
	//		auto& parent_children = nodes[parent].children_indices;
	//		parent_children.push_back(current_node_index);
	//	}
	//	else
	//	{
	//		nodes[current_node_index].parent_index = -1;
	//	}
	//}

	//void VkModelFactory::LoadVertAndIndxIntoGpuBuffer(GltfModelCI& model)
	//{
	//	const auto& index_buffer_cpu = model.index_buffer_cpu;
	//	const auto& vertex_buffer_cpu = model.vertex_buffer_cpu;

	//	auto& indices_gpu = model.indices_gpu;
	//	auto& vertices_gpu = model.vertices_gpu;

	//	//**************************************************************************************

	//	//vertex
	//	{
	//		const VkDeviceSize vertex_buffer_size = vertex_buffer_cpu.size() * sizeof(Gltf::Vertex);
	//		const auto         staging_buffer = buffer_factory.ProduceBufferReFac(vertex_buffer_size, CI::StagingBuffer);

	//		staging_buffer->CopyFromHost(vertex_buffer_cpu.data(), vertex_buffer_size);

	//		vertices_gpu.buffer = buffer_factory.ProduceBufferReFac(vertex_buffer_size, CI::VertexBuffer);
	//		vertices_gpu.count = static_cast<uint32_t>(vertex_buffer_cpu.size());

	//		vertices_gpu.buffer->CopyFromStagingBuf(staging_buffer, vertex_buffer_size);
	//	}

	//	//index
	//	{
	//		//TODO:�����size
	//		const VkDeviceSize index_buffer_size = sizeof(uint32_t) * index_buffer_cpu.size();
	//		const auto         staging_buffer = buffer_factory.ProduceBufferReFac(index_buffer_size, CI::StagingBuffer);

	//		staging_buffer->CopyFromHost(index_buffer_cpu.data(), index_buffer_size);

	//		indices_gpu.buffer = buffer_factory.ProduceBufferReFac(index_buffer_size, CI::IndexBuffer);
	//		indices_gpu.count = static_cast<uint32_t>(index_buffer_cpu.size());

	//		vertices_gpu.buffer->CopyFromStagingBuf(staging_buffer, index_buffer_size);
	//	}
	//}

	//bool VkModelFactory::TestIfGltfHadLoadedImg(const tinygltf::Image& glTFImage)
	//{
	//	const unsigned char* buffer = glTFImage.image.data();

	//	return !(nullptr == buffer);
	//}

}        // namespace Anni