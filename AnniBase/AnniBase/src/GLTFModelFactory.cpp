#include "GLTFModelFactory.h"

vk::Filter
Anni::GLTFModelFactory::ExtractFilter(fastgltf::Filter filter)
{
  switch (filter) {
      // nearest samplers
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
      return vk::Filter::eNearest;
      // linear samplers
    case fastgltf::Filter::Linear:
    case fastgltf::Filter::LinearMipMapNearest:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
      return vk::Filter::eLinear;
  }
}

vk::SamplerMipmapMode
Anni::GLTFModelFactory::ExtractMipmapMode(fastgltf::Filter filter)
{
  switch (filter) {
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
      return vk::SamplerMipmapMode::eNearest;

    case fastgltf::Filter::NearestMipMapLinear:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
      return vk::SamplerMipmapMode::eLinear;
  }
}
