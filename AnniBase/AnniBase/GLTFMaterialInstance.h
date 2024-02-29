#pragma once


#include "Mate.h"
#include "Buffer.h"
#include "VkTexture.h"


namespace Anni
{

	struct GLTFMaterialInstance
	{
		struct MaterialResources
		{
			std::shared_ptr<Buffer> dataBuffer;

			std::shared_ptr<VkTexture> albedoImage;
			std::shared_ptr<VkTexture> metalRoughImage;
			std::shared_ptr<VkTexture> normalImage;
			std::shared_ptr<VkTexture> emissiveImage;
			std::shared_ptr<VkTexture> occlusionImage;

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

