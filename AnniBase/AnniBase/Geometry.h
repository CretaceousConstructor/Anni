#pragma once

#include "AnniVkHeader.h"
#include "AnniMath.h"
#include <vector>

namespace Anni::Geometry
{
	struct VertexData
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec4 tangent;
		glm::vec4 color;
		glm::vec2 tex;
	};

	template <typename V, typename I>
	struct MeshData
	{
		std::vector<V> vertex_vec;
		std::vector<I> index_Vec;
	};

	MeshData<VertexData, uint32_t> CreateSphere(float radius, uint32_t levels, uint32_t slices, const glm::vec4& color);



}// namespace Anni::Geometry
