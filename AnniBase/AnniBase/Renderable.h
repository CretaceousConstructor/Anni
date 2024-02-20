#pragma once
#include "AnniMath.h"
#include "MeshAsset.h"
#include "RsrcUsage.h"
#include "GLTFMaterialInstance.h"
#include "GLTFMetallicRoughness.h"

namespace Anni
{

	struct RenderObject
	{
		uint32_t indexCount;
		uint32_t firstIndex;
		vk::DeviceAddress vertexBufferAddress;
		std::shared_ptr<Buffer> indexBuffer;

		Bounds bounds;
		glm::mat4 transform;

		MaterialInstance* material;
	};

	struct DrawContext
	{
		std::vector<RenderObject> OpaqueSurfaces;
		//std::vector<RenderObject> TransparentSurfaces;
	};




	class IRenderable
	{
		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
	};

	struct Node : public IRenderable
	{
		// parent pointer must be a weak pointer to avoid circular dependencies
		std::weak_ptr<Node> parent;
		std::vector<std::shared_ptr<Node>> children;

		glm::mat4 localTransform;
		glm::mat4 worldTransform;

		void refreshTransform(const glm::mat4& parentMatrix)
		{
			worldTransform = parentMatrix * localTransform;
			for (auto c : children)
			{
				c->refreshTransform(worldTransform);
			}
		}

		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx)
		{
			// draw children
			for (auto& c : children)
			{
				c->Draw(topMatrix, ctx);
			}
		}
	};

	struct LoadedGLTF : public IRenderable
	{
	public:
		LoadedGLTF(GLTFMetallicRoughnessProducer& metallic_roughness_) :
			metallic_roughness(metallic_roughness_)
		{
		}

		// storage for all the data on a given gltf file
		std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
		std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
		std::unordered_map<std::string, std::shared_ptr<VkTexture>> images;
		std::unordered_map<std::string, std::shared_ptr<GLTFMaterialInstance>> materials;

		std::vector<std::shared_ptr<VkTexture>> images_array;
		// nodes that dont have a parent, for iterating through the file in tree order
		std::vector<std::shared_ptr<Node>> topNodes;
		std::vector<std::shared_ptr<SamplerWrapper>> samplers;
		std::shared_ptr<Buffer> materialDataBuffer; // for all materials used in the model
		std::shared_ptr<DescriptorSetAllocatorGrowable> descriptor_allocator;
		std::string name{ "Shit" };

		GLTFMetallicRoughnessProducer& metallic_roughness;




		void UpdateDescriptorSet(DeviceManager& device_manager, const Anni::RenderGraphV1::TexUsage& all_tex_usage)
		{
			for (auto& mat : materials)
			{
				mat.second->UpdateDescriptorSet(device_manager, all_tex_usage);
			}
		}


		std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> BuildPipeline(GFXPipelineCI& gfx_pipe_CI)
		{
			return metallic_roughness.BuildPipelines(gfx_pipe_CI);
		}


		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx)
		{
			// create renderables from the scenenodes
			for (auto& n : topNodes)
			{
				n->Draw(topMatrix, ctx);
			}
		}


		//	void SetTextureUsage()
		//	{

		//	}

		//private:
		//	Anni::RenderGraphV1::TexUsage textures_usage;
	};

	struct MeshNode : public Node
	{
		std::shared_ptr<MeshAsset> mesh;
		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override
		{
			glm::mat4 nodeMatrix = topMatrix * worldTransform;

			for (auto& s : mesh->surfaces)
			{
				RenderObject def;
				def.indexCount = s.count;
				def.firstIndex = s.startIndex;
				def.indexBuffer = mesh->meshBuffers.indexBuffer;
				def.material = &s.material->data;
				def.bounds = s.bounds;
				def.transform = nodeMatrix;
				def.vertexBufferAddress = mesh->meshBuffers.vertexBufferAddress;

				//if (s.material->data.passType == MaterialPassType::Transparent)
				//{
				//	ctx.TransparentSurfaces.push_back(def);
				//}
				//else
				{
					ctx.OpaqueSurfaces.push_back(def);
				}
			}

			// recurse down
			Node::Draw(topMatrix, ctx);
		}
	};

}
