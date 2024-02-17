#pragma once

#include "GraphicsComponent.h"
#include "VkPipelinePP.h"



namespace Anni
{

	class IPipelineCIBuilder
	{
	public:
		IPipelineCIBuilder(GraphicsComponent& gfx_);
		virtual ~IPipelineCIBuilder() = default;

		IPipelineCIBuilder(const IPipelineCIBuilder&) = delete;
		IPipelineCIBuilder(IPipelineCIBuilder&&) = delete;

		IPipelineCIBuilder& operator=(const IPipelineCIBuilder&) = delete;
		IPipelineCIBuilder& operator=(IPipelineCIBuilder&&) = delete;

		std::unique_ptr<GFXPipelineCI> GetPipelineCI();
		void         RestoreToDefaultState(GFXPipelineCI& pp);


	protected:
		virtual void ConstructInputAssemblyStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructRasterizationStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructDepthStencilStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructMultisampleStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructDynamicState(GFXPipelineCI& pp) = 0;
		virtual void ConstructColorBlendStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructVertexInputStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructViewPortStateCI(GFXPipelineCI& pp) = 0;
		virtual void ConstructRenderingCI(GFXPipelineCI& pp) = 0;

	protected:
		GraphicsComponent& gfx;
		const SwapchainManager& swapchain_manager;
	};

}
