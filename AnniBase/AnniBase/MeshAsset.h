#pragma once
#include "Buffer.h"
#include "Mesh.h"
#include "GLTFMaterialInstance.h"

namespace Anni {

struct MeshAsset // 所有三角形的集合（比如模型的一个部件比如一个车轮，一个引擎）
{
  struct GeoSurface // 同材质所有的三角形的集合
  {
    uint32_t startIndex;
    uint32_t count;
    Bounds bounds;
    std::shared_ptr<GLTFMaterialInstance> material;
  };

  struct GPUMeshBuffers // 一个mesh用到的所有buffer
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
