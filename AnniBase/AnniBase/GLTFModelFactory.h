#pragma once
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>

#include "BufferFactory.h"
#include "MeshAsset.h"
#include "Renderable.h"
#include "TextureFactory.h"
#include "GLTFMaterialInstance.h"
#include "GLTFMetallicRoughness.h"


namespace Anni {

class GLTFModelFactory
{
private:
  static vk::Filter ExtractFilter(fastgltf::Filter filter);
  static vk::SamplerMipmapMode ExtractMipmapMode(fastgltf::Filter filter);

public:
  GLTFModelFactory(DeviceManager& device_manager_,
                   BufferFactory& buf_fac_,
                   VkTextureFactory& tex_fac_,
                   GLTFMetallicRoughnessProducer& mat_metallic_roughness_)
    : device_manager(device_manager_)
    , buf_fac(buf_fac_)
    , tex_fac(tex_fac_)
    , mat_metallic_roughness_producer(mat_metallic_roughness_)
  {
    //> des_allocator_init init descriptor allocator
    std::vector<DescriptorSetAllocatorGrowable::PoolSizeRatio> sizes = {
      { vk::DescriptorType::eCombinedImageSampler, 5. },
      { vk::DescriptorType::eUniformBuffer, 1. },
      { vk::DescriptorType::eStorageBuffer, 1. }
    };

    //5 is an arbitrary magic number
    descriptor_allocator = std::make_shared<DescriptorSetAllocatorGrowable>(device_manager, 5u, sizes);


  }

  std::shared_ptr<LoadedGLTF> LoadGLTF(std::string_view filePath)
  {
    std::cout << "Loading GLTF: " << filePath;
    //> load_1 gltf file in
    std::shared_ptr<LoadedGLTF> gltf_scene = std::make_shared<LoadedGLTF>(mat_metallic_roughness_producer);
    LoadedGLTF& result_gltf = *gltf_scene;

    fastgltf::Parser parser{};

    constexpr auto gltfOptions =
      fastgltf::Options::DontRequireValidAssetMember |
      fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers |
      fastgltf::Options::LoadExternalBuffers;
    // fastgltf::Options::LoadExternalImages;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(filePath);

    fastgltf::Asset gltf;
    std::filesystem::path path = filePath;

    //> load_raw gltf raw file loading
    auto type = fastgltf::determineGltfFileType(&data);
    if (type == fastgltf::GltfType::glTF) {
      auto load = parser.loadGLTF(&data, path.parent_path(), gltfOptions);
      if (load) {
        gltf = std::move(load.get());
      } else {
        std::cerr << "Failed to load glTF: "
                  << fastgltf::to_underlying(load.error()) << std::endl;
        assert(false);
      }
    } else if (type == fastgltf::GltfType::GLB) {
      auto load = parser.loadBinaryGLTF(&data, path.parent_path(), gltfOptions);
      if (load) {
        gltf = std::move(load.get());
      } else {
        std::cerr << "Failed to load glTF: "
                  << fastgltf::to_underlying(load.error()) << std::endl;
        assert(false);
      }
    } else {
      std::cerr << "Failed to determine glTF container" << std::endl;
      assert(false);
    }
    //< load_raw

    //> des_allocator_init init descriptor allocator
 
    //5 is a, arbitrary magic number
    result_gltf.descriptor_allocator = descriptor_allocator;

	//< des_allocator_init


    //> load_SAMPLERS
    for (fastgltf::Sampler& sampler : gltf.samplers) {
      vk::SamplerCreateInfo sampl_CI = CI::PopulateTexSamplerCI();
      sampl_CI.maxLod = VK_LOD_CLAMP_NONE;
      sampl_CI.minLod = 0;

      sampl_CI.magFilter =
        ExtractFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
      sampl_CI.minFilter =
        ExtractFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

      sampl_CI.mipmapMode = ExtractMipmapMode(
        sampler.minFilter.value_or(fastgltf::Filter::Nearest));

      std::shared_ptr<SamplerWrapper> ptr_sampler =
        std::make_shared<SamplerWrapper>(device_manager, sampl_CI);
      result_gltf.samplers.push_back(ptr_sampler);
    }
    //< load_SAMPLERS


    //> create_temp_arrays
    // temporal arrays for all the objects to use while creating the GLTF data
    std::vector<std::shared_ptr<MeshAsset>> meshes;
    std::vector<std::shared_ptr<Node>> nodes;
    // std::vector<std::shared_ptr<VkTexture>> images;
    std::vector<std::shared_ptr<GLTFMaterialInstance>> materials;
    //< create_temp_arrays


    //> load all TEXTURES
    for (fastgltf::Image& image : gltf.images) {
      std::shared_ptr<VkTexture> img = load_image(gltf, image);
      if (img) {
        result_gltf.images_array.push_back(img);
        result_gltf.images[image.name.c_str()] = img;
      } 
      else
      {
        // we failed to load, so lets give the slot a default white texture to
        // not completely break loading
        // images.push_back(engine->_errorCheckerboardImage);
        // std::cout << "gltf failed to load texture " << image.name <<
        // std::endl; throw std::runtime_error("img failed to load!");
        assert(false);
      }
    }
    //< load all TEXTURES


    //> load_BUFFER
    // create buffer to hold all material constants data
    result_gltf.materialDataBuffer = buf_fac.ProduceBuffer(sizeof(GLTFMaterialInstance::MaterialConstants) * gltf.materials.size(),CI::UniformBuffer);
    //< load_BUFFER

    //> load_MATERIAL

	for (const auto& [mat_index, mat] : std::ranges::views::enumerate(gltf.materials))
    {
      std::shared_ptr<GLTFMaterialInstance> newMat = std::make_shared<GLTFMaterialInstance>();
      materials.push_back(newMat);
      result_gltf.materials[mat.name.c_str()] = newMat;

      GLTFMaterialInstance::MaterialConstants constants;
      constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
      constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
      constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
      constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

      constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
      constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;

      // write material parameters to buffer
      // sceneMaterialConstants[mat_index] = constants;
      result_gltf.materialDataBuffer->CopyFromHost(
        (void const*)&constants,
        sizeof(GLTFMaterialInstance::MaterialConstants),
        mat_index * sizeof(GLTFMaterialInstance::MaterialConstants));

      MaterialPassType passType = MaterialPassType::MainColor;
      //if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
      //  passType = MaterialPassType::Transparent;
      //}

      GLTFMaterialInstance::MaterialResources materialResources;
      // default the material textures
      // materialResources.albedoImage = engine->_whiteImage;
      // materialResources.colorSampler = engine->_defaultSamplerLinear;
      // materialResources.metalRoughImage = engine->_whiteImage;
      // materialResources.metalRoughSampler = engine->_defaultSamplerLinear;

      // set the uniform buffer for the material constant data
      materialResources.dataBuffer = result_gltf.materialDataBuffer;
      materialResources.dataBufferOffset =
        mat_index * sizeof(GLTFMaterialInstance::MaterialConstants);
      // grab textures from gltf file
      if (mat.pbrData.baseColorTexture.has_value()) {
        size_t img =
          gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex]
            .imageIndex.value();
        size_t sampler =
          gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex]
            .samplerIndex.value();

        materialResources.albedoImage = result_gltf.images_array[img];
        materialResources.albedoImage->ResetSampler(result_gltf.samplers[sampler]);
        // materialResources.colorSampler = file.samplers[sampler];
      }

      if (mat.pbrData.metallicRoughnessTexture.has_value()) {
        size_t img =
          gltf
            .textures[mat.pbrData.metallicRoughnessTexture.value().textureIndex]
            .imageIndex.value();
        size_t sampler =
          gltf
            .textures[mat.pbrData.metallicRoughnessTexture.value().textureIndex]
            .samplerIndex.value();

        materialResources.metalRoughImage = result_gltf.images_array[img];
        materialResources.metalRoughImage->ResetSampler(
          result_gltf.samplers[sampler]);
      }

      if (mat.normalTexture.has_value()) {
        size_t img = gltf.textures[mat.normalTexture.value().textureIndex]
                       .imageIndex.value();
        size_t sampler = gltf.textures[mat.normalTexture.value().textureIndex]
                           .samplerIndex.value();

        materialResources.normalImage = result_gltf.images_array[img];
        materialResources.normalImage->ResetSampler(
          result_gltf.samplers[sampler]);
      }

      if (mat.emissiveTexture.has_value()) {
        size_t img = gltf.textures[mat.emissiveTexture.value().textureIndex]
                       .imageIndex.value();
        size_t sampler = gltf.textures[mat.emissiveTexture.value().textureIndex]
                           .samplerIndex.value();

        materialResources.emissiveImage = result_gltf.images_array[img];
        materialResources.emissiveImage->ResetSampler(
          result_gltf.samplers[sampler]);
      }

      if (mat.occlusionTexture.has_value()) {
        size_t img = gltf.textures[mat.occlusionTexture.value().textureIndex]
                       .imageIndex.value();
        size_t sampler =
          gltf.textures[mat.occlusionTexture.value().textureIndex]
            .samplerIndex.value();

        materialResources.occlusionImage = result_gltf.images_array[img];
        materialResources.occlusionImage->ResetSampler(
          result_gltf.samplers[sampler]);
      }

      // instantiate material
      newMat->data = mat_metallic_roughness_producer.Instantiate(passType, *result_gltf.descriptor_allocator);
      // assign material resources
      newMat->materialResources = materialResources;

    }
    //< load_material

    // use the same vectors for all meshes so that the memory doesnt reallocate
    // as often
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    for (fastgltf::Mesh& mesh : gltf.meshes) {
      std::shared_ptr<MeshAsset> newmesh = std::make_shared<MeshAsset>();
      meshes.push_back(newmesh);
      result_gltf.meshes[mesh.name.c_str()] = newmesh;
      newmesh->name = mesh.name;

      // clear the mesh arrays each mesh, we dont want to merge them by error
      indices.clear();
      vertices.clear();

      for (auto&& p : mesh.primitives) {
        MeshAsset::GeoSurface newSurface;
        newSurface.startIndex = (uint32_t)indices.size();
        newSurface.count =
          (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

        size_t initial_vtx = vertices.size();

        // load indexes
        {
          fastgltf::Accessor& indexaccessor =
            gltf.accessors[p.indicesAccessor.value()];
          indices.reserve(indices.size() + indexaccessor.count);

          fastgltf::iterateAccessor<std::uint32_t>(
            gltf, indexaccessor, [&](std::uint32_t idx) {
              indices.push_back(idx + initial_vtx);
            });
        }

        // load vertex positions
        {
          fastgltf::Accessor& posAccessor =
            gltf.accessors[p.findAttribute("POSITION")->second];
          vertices.resize(vertices.size() + posAccessor.count);

          fastgltf::iterateAccessorWithIndex<glm::vec3>(
            gltf, posAccessor, [&](glm::vec3 v, size_t index) -> void {
              Vertex newvtx;
              newvtx.position = v;
              newvtx.normal = { 1, 0, 0 };
              newvtx.color = glm::vec4{ 1.f };
              newvtx.uv_x = 0;
              newvtx.uv_y = 0;
              vertices[initial_vtx + index] = newvtx;
            });
        }

        // load vertex normals
        auto normals = p.findAttribute("NORMAL");
        if (normals != p.attributes.end()) {
          fastgltf::iterateAccessorWithIndex<glm::vec3>(
            gltf,
            gltf.accessors[(*normals).second],
            [&](glm::vec3 v, size_t index) {
              vertices[initial_vtx + index].normal = v;
            });
        }

        // load UVs
        auto uv = p.findAttribute("TEXCOORD_0");
        if (uv != p.attributes.end()) {
          fastgltf::iterateAccessorWithIndex<glm::vec2>(
            gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v, size_t index) {
              vertices[initial_vtx + index].uv_x = v.x;
              vertices[initial_vtx + index].uv_y = v.y;
            });
        }

        // load vertex colors
        auto colors = p.findAttribute("COLOR_0");
        if (colors != p.attributes.end()) {
          fastgltf::iterateAccessorWithIndex<glm::vec4>(
            gltf,
            gltf.accessors[(*colors).second],
            [&](glm::vec4 v, size_t index) {
              vertices[initial_vtx + index].color = v;
            });
        }

        // load material index
        if (p.materialIndex.has_value()) {
          newSurface.material = materials[p.materialIndex.value()];
        } else {
          newSurface.material = materials[0];
        }

        glm::vec3 minpos = vertices[initial_vtx].position;
        glm::vec3 maxpos = vertices[initial_vtx].position;
        for (int i = initial_vtx; i < vertices.size(); i++) {
          minpos = glm::min(minpos, vertices[i].position);
          maxpos = glm::max(maxpos, vertices[i].position);
        }

        newSurface.bounds.origin = (maxpos + minpos) / 2.f;
        newSurface.bounds.extents = (maxpos - minpos) / 2.f;
        newSurface.bounds.sphereRadius = glm::length(newSurface.bounds.extents);
        newmesh->surfaces.push_back(newSurface);
      }
      newmesh->meshBuffers = upload_mesh(indices, vertices);
    }

    //> load_nodes
    // load all nodes and their meshes

    for (fastgltf::Node& node : gltf.nodes) {
      std::shared_ptr<Node> newNode;

      // find if the node has a mesh, and if it does hook it to the mesh pointer
      // and allocate it with the meshnode class
      if (node.meshIndex.has_value()) {
        newNode = std::make_shared<MeshNode>();
        static_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
      } else {
        newNode = std::make_shared<Node>();
      }

      nodes.push_back(newNode);
      result_gltf.nodes[node.name.c_str()] = newNode;

      std::visit(
        fastgltf::visitor{
          [&](fastgltf::Node::TransformMatrix matrix) {
            memcpy(&newNode->localTransform, matrix.data(), sizeof(matrix));
          },

          [&](fastgltf::Node::TRS transform) {
            glm::vec3 tl(transform.translation[0],
                         transform.translation[1],
                         transform.translation[2]);
            glm::quat rot(transform.rotation[3],
                          transform.rotation[0],
                          transform.rotation[1],
                          transform.rotation[2]);
            glm::vec3 sc(
              transform.scale[0], transform.scale[1], transform.scale[2]);

            glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
            glm::mat4 rm = glm::toMat4(rot);
            glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

            newNode->localTransform = tm * rm * sm;
          } },
        node.transform);
    }
    //< load_nodes

    //> load_graph
    // run loop again to setup transform hierarchy
    for (int i = 0; i < gltf.nodes.size(); i++) {
      fastgltf::Node& node = gltf.nodes[i];
      std::shared_ptr<Node>& sceneNode = nodes[i];

      for (auto& c : node.children) {
        sceneNode->children.push_back(nodes[c]);
        nodes[c]->parent = sceneNode;
      }
    }

    // find the top nodes, with no parents
    for (auto& node : nodes) {
      if (node->parent.lock() == nullptr) {
        result_gltf.topNodes.push_back(node);
        node->refreshTransform(glm::mat4{ 1.f });
      }
    }
    return gltf_scene;
    //< load_graph
  }

  std::shared_ptr<VkTexture> load_image(fastgltf::Asset& asset,
                                        fastgltf::Image& image)
  {
    int width, height, nrChannels;
    std::shared_ptr<VkTexture> result_image;
    std::visit(
      fastgltf::visitor{
        [](auto& arg) {},
        [&](fastgltf::sources::URI& filePath) {
          assert(filePath.fileByteOffset ==
                 0); // We don't support offsets with stbi.
          assert(filePath.uri.isLocalPath()); // We're only capable of
          // loading local files.

          const std::string path(filePath.uri.path().begin(),
                                 filePath.uri.path().end()); // Thanks C++.
          unsigned char* data =
            stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
          if (data) {
            VkExtent3D img_extend;
            img_extend.width = width;
            img_extend.height = height;
            img_extend.depth = 1;

            constexpr auto format_of_image =
              vk::Format::eR8G8B8A8Unorm; // TODO: 难崩，怎么用UNORM读的
            const auto img_view_CI =
              CI::PopulateTextureImgViewCI(format_of_image);
            const auto tex_img_CI =
              CI::GetTextureImgCI(format_of_image,
                                  img_extend,
                                  vk::ImageCreateFlags(VK_ZERO_FLAG),
                                  path);

            result_image = tex_fac.ProduceTextureFromBufferReFac(
              tex_img_CI, std::nullopt, img_view_CI, data);
            stbi_image_free(data);
          }
        },
        [&](fastgltf::sources::Vector& vector) {
          unsigned char* data =
            stbi_load_from_memory(vector.bytes.data(),
                                  static_cast<int>(vector.bytes.size()),
                                  &width,
                                  &height,
                                  &nrChannels,
                                  4);
          if (data) {
            VkExtent3D image_extend;
            image_extend.width = width;
            image_extend.height = height;
            image_extend.depth = 1;

            constexpr auto format_of_image =
              vk::Format::eR8G8B8A8Unorm; // TODO: 难崩，怎么用UNORM读的
            const auto img_view_CI =
              CI::PopulateTextureImgViewCI(format_of_image);
            const auto tex_img_CI =
              CI::GetTextureImgCI(format_of_image, image_extend);

            result_image = tex_fac.ProduceTextureFromBufferReFac(
              tex_img_CI, std::nullopt, img_view_CI, data);
            stbi_image_free(data);
          }
        },

        [&](fastgltf::sources::BufferView& view) {
          auto& bufferView = asset.bufferViews[view.bufferViewIndex];
          auto& buffer = asset.buffers[bufferView.bufferIndex];

          std::visit(
            fastgltf::visitor{
              // We only care about VectorWithMime here, because we
              // specify LoadExternalBuffers, meaning all buffers
              // are already loaded into a vector.
              [](auto& arg) {},
              [&](fastgltf::sources::Vector& vector) {
                unsigned char* data = stbi_load_from_memory(
                  vector.bytes.data() + bufferView.byteOffset,
                  static_cast<int>(bufferView.byteLength),
                  &width,
                  &height,
                  &nrChannels,
                  4);
                if (data) {
                  VkExtent3D image_extend;
                  image_extend.width = width;
                  image_extend.height = height;
                  image_extend.depth = 1;

                  constexpr auto format_of_image =
                    vk::Format::eR8G8B8A8Unorm; // TODO: 难崩，怎么用UNORM读的
                  const auto img_view_CI =
                    CI::PopulateTextureImgViewCI(format_of_image);
                  const auto tex_img_CI =
                    CI::GetTextureImgCI(format_of_image, image_extend);

                  result_image = tex_fac.ProduceTextureFromBufferReFac(
                    tex_img_CI, std::nullopt, img_view_CI, data);

                  stbi_image_free(data);
                }
              } },
            buffer.data);
        },
      },
      image.data);

    // if any of the attempts to load the data failed, we havent written the
    // image so handle is null
    return result_image;
  }

  MeshAsset::GPUMeshBuffers upload_mesh(std::span<const uint32_t> indices,
                                        std::span<const Vertex> vertices)
  {
    const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
    const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

    MeshAsset::GPUMeshBuffers buffer_4_mesh;

    buffer_4_mesh.vertexBuffer =
      buf_fac.ProduceBuffer(vertexBufferSize, CI::StorageBufferAddressable);
    vk::BufferDeviceAddressInfo deviceAdressInfo{};
    deviceAdressInfo.setBuffer(buffer_4_mesh.vertexBuffer->GetGPUBuffer());

    buffer_4_mesh.vertexBufferAddress =
        device_manager.GetLogicalDevice().getBufferAddress(&deviceAdressInfo);

    buffer_4_mesh.indexBuffer =
      buf_fac.ProduceBuffer(indexBufferSize, CI::IndexBuffer);

    auto staging_buf = buf_fac.ProduceBuffer(vertexBufferSize + indexBufferSize,
                                             CI::StagingBuffer);
    // void* mapped_data_vert = staging_buf->GetGPUBuffer();
    // void* mapped_data_index = (char*)mapped_data_vert + vertexBufferSize;

    // copy vertex buffer
    staging_buf->CopyFromHost(vertices.data(), vertexBufferSize, 0);
    // copy index buffer
    staging_buf->CopyFromHost(
      indices.data(), indexBufferSize, vertexBufferSize);

    Buf2BufCopyInfo copy_inf;
    copy_inf.size = vertexBufferSize;
    copy_inf.srcOffset = 0u;
    copy_inf.dstOffset = 0u;

    buffer_4_mesh.vertexBuffer->CopyFromStagingBuf(*staging_buf, copy_inf);

    copy_inf.size = indexBufferSize;
    copy_inf.srcOffset = vertexBufferSize;
    copy_inf.dstOffset = 0u;

    buffer_4_mesh.indexBuffer->CopyFromStagingBuf(*staging_buf, copy_inf);
    return buffer_4_mesh;
  }

private:
  DeviceManager& device_manager;
  BufferFactory& buf_fac;
  VkTextureFactory& tex_fac;
  GLTFMetallicRoughnessProducer& mat_metallic_roughness_producer;
  std::shared_ptr<DescriptorSetAllocatorGrowable>  descriptor_allocator;

};

}
