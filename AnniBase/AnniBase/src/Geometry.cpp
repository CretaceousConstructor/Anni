#include "Geometry.h"

namespace Anni::Geometry
{
	MeshData<VertexData, uint32_t> Geometry::CreateSphere(float radius, uint32_t levels, uint32_t slices, const glm::vec4& color)
	{
		MeshData<VertexData, uint32_t> mesh_data;

		uint32_t vertex_count = 2 + (levels - 1) * (slices + 1);
		uint32_t index_count = 6 * (levels - 1) * slices;
		mesh_data.vertex_vec.resize(vertex_count);
		mesh_data.index_Vec.resize(index_count);

		VertexData vertex_data;
		uint32_t   vIndex = 0, iIndex = 0;

		float phi = 0.0f, theta = 0.0f;
		float per_phi = glm::pi<float>() / levels;
		float per_theta = (2.f * glm::pi<float>()) / slices;
		float x, y, z;

		// 放入顶端点
		vertex_data = { glm::vec3(0.0f, radius, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), color, glm::vec2(0.0f, 0.0f) };
		mesh_data.vertex_vec[vIndex++] = vertex_data;

		for (uint32_t i = 1; i < levels; ++i)
		{
			phi = per_phi * i;
			// 需要slices + 1个顶点是因为 起点和终点需为同一点，但纹理坐标值不一致
			for (uint32_t j = 0; j <= slices; ++j)
			{
				theta = per_theta * j;
				x = radius * sinf(phi) * cosf(theta);
				y = radius * cosf(phi);
				z = radius * sinf(phi) * sinf(theta);
				// 计算出局部坐标、法向量、Tangent向量和纹理坐标
				glm::vec3 pos = glm::vec3(x, y, z);
				glm::vec3 normal;
				normal = glm::normalize(pos);

				vertex_data = { pos, normal, glm::vec4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, glm::vec2(theta / (2.f * glm::pi<float>()), phi / glm::pi<float>()) };
				mesh_data.vertex_vec[vIndex++] = vertex_data;
			}
		}

		// 放入底端点
		vertex_data = { glm::vec3(0.0f, -radius, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
					   glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f), color, glm::vec2(0.0f, 1.0f) };
		mesh_data.vertex_vec[vIndex++] = vertex_data;

		// 逐渐放入索引
		if (levels > 1)
		{
			for (uint32_t j = 1; j <= slices; ++j)
			{
				mesh_data.index_Vec[iIndex++] = 0;
				mesh_data.index_Vec[iIndex++] = j % (slices + 1) + 1;
				mesh_data.index_Vec[iIndex++] = j;
			}
		}

		for (uint32_t i = 1; i < levels - 1; ++i)
		{
			for (uint32_t j = 1; j <= slices; ++j)
			{
				mesh_data.index_Vec[iIndex++] = (i - 1) * (slices + 1) + j;
				mesh_data.index_Vec[iIndex++] = (i - 1) * (slices + 1) + j % (slices + 1) + 1;
				mesh_data.index_Vec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;

				mesh_data.index_Vec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;
				mesh_data.index_Vec[iIndex++] = i * (slices + 1) + j;
				mesh_data.index_Vec[iIndex++] = (i - 1) * (slices + 1) + j;
			}
		}

		// 逐渐放入索引
		if (levels > 1)
		{
			for (uint32_t j = 1; j <= slices; ++j)
			{
				mesh_data.index_Vec[iIndex++] = (levels - 2) * (slices + 1) + j;
				mesh_data.index_Vec[iIndex++] = (levels - 2) * (slices + 1) + j % (slices + 1) + 1;
				mesh_data.index_Vec[iIndex++] = (levels - 1) * (slices + 1) + 1;
			}
		}

		return mesh_data;
	}
}        // namespace Geometry
