#pragma once

#include "Buffer.h"
#include "AnniMath.h"
#include "Texture.h"

namespace Anni
{
	namespace Gltf
	{
		struct Vertex
		{
			glm::vec4                                             tangent;
			glm::vec3                                             pos;
			glm::vec3                                             normal;
			glm::vec3                                             color;
			glm::vec2                                             uv;
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};



		struct Primitive
		{
			uint32_t fir_idx;
			uint32_t idx_count;
			uint32_t material_idx;
		};

		struct Mesh
		{
			std::vector<Primitive> primitives;
		};

		struct Node
		{
			int              parent_index;        //index ito the std::vector<GltfModel::Node> nodes vector
			std::vector<int> children_indices;
			Mesh             mesh;
			glm::mat4        matrix;
			bool             visible{ true };
		};

		using Image = Texture;

		struct Texture
		{
			uint32_t image_idx;
		};

		// Single vertex buffer for all primitives
		struct VertexBufferGpu
		{
			size_t                  count;
			std::shared_ptr<Buffer> buffer;
		};

		// Single index buffer for all primitives
		struct IndexBufferGpu
		{
			size_t                  count;
			std::shared_ptr<Buffer> buffer;
		};
	}        // namespace Gltf

}        // namespace Anni
