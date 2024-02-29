#include "Renderable.h"

void Anni::Node::refreshTransform(const glm::mat4& parentMatrix)
{
	worldTransform = parentMatrix * localTransform;
	for (auto c : children)
	{
		c->refreshTransform(worldTransform);
	}
}

void Anni::Node::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	// draw children
	for (auto& c : children)
	{
		c->Draw(topMatrix, ctx);
	}
}

Anni::LoadedGLTF::LoadedGLTF(GLTFMetallicRoughnessProducer& metallic_roughness_):
	metallic_roughness(metallic_roughness_)
{
}

void Anni::LoadedGLTF::UpdateDescriptorSet(DeviceManager& device_manager,
	const Anni::RenderGraphV1::TexUsage& all_tex_usage) const
{
	for (auto& mat : materials)
	{
		mat.second->UpdateDescriptorSet(device_manager, all_tex_usage);
	}
}

std::pair<vk::UniquePipeline, vk::UniquePipelineLayout> Anni::LoadedGLTF::BuildPipeline(
	const GFXPipelineCI& gfx_pipe_CI) const
{
	return metallic_roughness.BuildPipelines(gfx_pipe_CI);
}

void Anni::LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	// create renderables from the scenenodes
	for (auto& n : topNodes)
	{
		n->Draw(topMatrix, ctx);
	}
}

void Anni::MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
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
