#pragma once
#include "IPipelineCIBuilder.h"

#include "VkPipelinePP.h"

namespace Anni
{
	class GFXPipelineCIBuilder final : public IPipelineCIBuilder
	{
	public:
		GFXPipelineCIBuilder(GraphicsComponent& gfx_);
		~GFXPipelineCIBuilder() override = default;

		GFXPipelineCIBuilder(const GFXPipelineCIBuilder&) = delete;
		GFXPipelineCIBuilder(GFXPipelineCIBuilder&&) = delete;

		GFXPipelineCIBuilder& operator=(const GFXPipelineCIBuilder&) = delete;
		GFXPipelineCIBuilder& operator=(GFXPipelineCIBuilder&&) = delete;

	protected:
		void ConstructInputAssemblyStateCI(GFXPipelineCI& CI) override;
		void ConstructRasterizationStateCI(GFXPipelineCI& CI) override;
		void ConstructDepthStencilStateCI(GFXPipelineCI& CI) override;
		void ConstructMultisampleStateCI(GFXPipelineCI& CI) override;
		void ConstructDynamicState(GFXPipelineCI& CI) override;
		void ConstructColorBlendStateCI(GFXPipelineCI& CI) override;
		void ConstructVertexInputStateCI(GFXPipelineCI& CI) override;
		void ConstructViewPortStateCI(GFXPipelineCI& CI) override;
		void ConstructRenderingCI(GFXPipelineCI& CI) override;
	};


}

