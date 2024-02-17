#include "IPipelineCIBuilder.h"


namespace Anni
{
	IPipelineCIBuilder::IPipelineCIBuilder(GraphicsComponent& gfx_) :
		gfx(gfx_),
		swapchain_manager(gfx.SwapchainMan())
	{
	}

	std::unique_ptr<GFXPipelineCI> IPipelineCIBuilder::GetPipelineCI()
	{
		auto result{ std::make_unique<GFXPipelineCI>() };

		ConstructInputAssemblyStateCI(*result);
		ConstructRasterizationStateCI(*result);
		ConstructDepthStencilStateCI(*result);
		ConstructMultisampleStateCI(*result);
		ConstructDynamicState(*result);
		ConstructColorBlendStateCI(*result);
		ConstructVertexInputStateCI(*result);
		ConstructViewPortStateCI(*result);
		ConstructRenderingCI(*result);

		return result;
	}

	void IPipelineCIBuilder::RestoreToDefaultState(GFXPipelineCI& CI)
	{
		CI.ClearVectors();

		ConstructInputAssemblyStateCI(CI);
		ConstructRasterizationStateCI(CI);
		ConstructDepthStencilStateCI(CI);
		ConstructMultisampleStateCI(CI);
		ConstructDynamicState(CI);
		ConstructColorBlendStateCI(CI);
		ConstructVertexInputStateCI(CI);
		ConstructViewPortStateCI(CI);
		ConstructRenderingCI(CI);
	}

}
