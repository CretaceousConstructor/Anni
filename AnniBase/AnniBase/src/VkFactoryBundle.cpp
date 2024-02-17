#include "VkFactoryBundle.h"


namespace Anni
{

	VkFactoryBundle::VkFactoryBundle(GraphicsComponent& gfx) :
		buffer_factory(gfx),
		image_factory(gfx),
		syn_obj_factory(gfx),
		//descriptor_set_factory(gfx),
		shader_factory(gfx),
		texture_factory(gfx, image_factory, buffer_factory, sampler_factory, img_view_factory),
		//model_factory(gfx, texture_factory, buffer_factory),
		forward_shading_PP_factory(gfx)
	{




	}

}
