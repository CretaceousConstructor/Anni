#pragma once


#include "Mate.h"
#include "Buffer.h"
#include "Texture.h"


namespace Anni
{

	struct GLTFMaterialInstance
	{
		struct MaterialResources
		{
			std::shared_ptr<Buffer> dataBuffer;

			std::shared_ptr<Texture> albedoImage;
			std::shared_ptr<Texture> metalRoughImage;
			std::shared_ptr<Texture> normalImage;
			std::shared_ptr<Texture> emissiveImage;
			std::shared_ptr<Texture> occlusionImage;

			uint32_t dataBufferOffset{};
		};

		struct MaterialConstants
		{
			glm::vec4 colorFactors;
			glm::vec4 metal_rough_factors;
			//padding, we need it anyway for uniform buffers
			glm::vec4 extra[14];
		};


		MaterialInstance data;
		MaterialResources materialResources;

		void UpdateDescriptorSet(DeviceManager& device_manager, const RenderGraphV1::TexUsage& tex_usage);
	};
}

