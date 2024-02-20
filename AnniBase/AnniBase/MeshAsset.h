#pragma once
#include "Buffer.h"
#include "Mesh.h"
#include "GLTFMaterialInstance.h"

namespace Anni {

struct MeshAsset // ���������εļ��ϣ�����ģ�͵�һ����������һ�����֣�һ�����棩
{
  struct GeoSurface // ͬ�������е������εļ���
  {
    uint32_t startIndex;
    uint32_t count;
    Bounds bounds;
    std::shared_ptr<GLTFMaterialInstance> material;
  };

  struct GPUMeshBuffers // һ��mesh�õ�������buffer
  {
    std::shared_ptr<Anni::Buffer> indexBuffer;
    std::shared_ptr<Anni::Buffer> vertexBuffer;
    vk::DeviceAddress vertexBufferAddress;
  };

  // holds the resources needed for a mesh
  std::string name;
  std::vector<GeoSurface> surfaces; // material
  GPUMeshBuffers meshBuffers;       // buffer
};

}
