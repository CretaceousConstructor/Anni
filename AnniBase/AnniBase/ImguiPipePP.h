#pragma once

//#include "GFXPipelineCI.h"
//
//class ImguiPipePP : public GFXPipelineCI
//
//{
//public:
//	ImguiPipePP(GraphicsComponent &gfx_);
//
//	void InitInputAssemblyStateCI() override final;
//
//	void InitRasterizationStateCI() override final;
//
//	void InitDepthStencilStateCI() override final;
//
//	void InitMultisampleStateCI() override final;
//
//	void InitDynamicState() override final;
//
//	void InitColorBlendStateCI() override final;
//
//	void InitViewPortStateCI() override final;
//
//	void InitVertexInputStateCI() override final;
//
//	void InitRenderingCI() override final;
//
//	[[nodiscard]]std::shared_ptr<GFXPipelineCI> Clone() const override final;
//
//};



#include "VkPipelinePPFactory.h"

namespace Anni
{
	class ImguiPipePPFactory final : public IPipelineCIBuilder
	{
	public:
		ImguiPipePPFactory(GraphicsComponent& gfx_);
		~ImguiPipePPFactory() override = default;

		ImguiPipePPFactory(const ImguiPipePPFactory&) = delete;
		ImguiPipePPFactory(ImguiPipePPFactory&&) = delete;

		ImguiPipePPFactory& operator=(const ImguiPipePPFactory&) = delete;
		ImguiPipePPFactory& operator=(ImguiPipePPFactory&&) = delete;

	protected:
		void ConstructInputAssemblyStateCI(GFXPipelineCI& pp) override;
		void ConstructRasterizationStateCI(GFXPipelineCI& pp) override;
		void ConstructDepthStencilStateCI(GFXPipelineCI& pp) override;
		void ConstructMultisampleStateCI(GFXPipelineCI& pp) override;
		void ConstructDynamicState(GFXPipelineCI& pp) override;
		void ConstructColorBlendStateCI(GFXPipelineCI& pp) override;
		void ConstructVertexInputStateCI(GFXPipelineCI& pp) override;
		void ConstructViewPortStateCI(GFXPipelineCI& pp) override;
		void ConstructRenderingCI(GFXPipelineCI& pp) override;



	};
}

