#pragma once

#include "BufferFactory.h"
#include "VkDescriptorManager.h"
#include "ImageFactory.h"
#include "VkModelFactory.h"
//#include "VkSubpassFactory.h"
#include "VkShaderFactory.h"
#include "VkSynObjectFactory.h"
#include "TextureFactory.h"

//#include "ForwardShadingPipelinePP.h"


namespace Anni
{

	class VkFactoryBundle
	{
		friend class VkRenderpassManager;

	private:
		VkFactoryBundle(GraphicsComponent& gfx);

		//VkSubPassFacotry        subpass_factory;
		//VkTexImageFactory       tex_image_factory;
		//VkSwapchainImageFactory swapchain_factory;
		//VkDepthImageFactory     depth_image_factory;

		//FACTORIES
		BufferFactory          buffer_factory;
		ImageFactory           image_factory;
		VkSynObjectFactory     syn_obj_factory;
		//VkDescriptorSetFactory descriptor_set_factory;
		VkShaderFactory        shader_factory;
		VkTextureFactory       texture_factory;
		//VkModelFactory         model_factory;

	public:
		//There will be a lot of different pipeline pp factories in the future, so simply make them public
		GFXPipelineCIBuilder forward_shading_PP_factory;
	};

}
