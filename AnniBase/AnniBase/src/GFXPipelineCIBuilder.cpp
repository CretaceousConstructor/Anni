
#include "GFXPipelineCIBuilder.h"


namespace Anni
{

	GFXPipelineCIBuilder::GFXPipelineCIBuilder(GraphicsComponent& gfx_) :
		IPipelineCIBuilder(gfx_)
	{
	}

	void GFXPipelineCIBuilder::ConstructInputAssemblyStateCI(GFXPipelineCI& pp)
	{
		pp.input_assembly_state_CI.primitiveRestartEnable = vk::False;
	}

	void GFXPipelineCIBuilder::ConstructRasterizationStateCI(GFXPipelineCI& pp)
	{
		pp.rasterization_state_CI.depthClampEnable = vk::False;
		pp.rasterization_state_CI.rasterizerDiscardEnable = vk::False;

		pp.rasterization_state_CI.polygonMode = vk::PolygonMode::eFill;
		pp.rasterization_state_CI.lineWidth = 1.f;
		
		pp.rasterization_state_CI.cullMode = vk::CullModeFlagBits::eBack;
		pp.rasterization_state_CI.frontFace = vk::FrontFace::eCounterClockwise;

		pp.rasterization_state_CI.depthBiasEnable = vk::False;
		pp.rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
		pp.rasterization_state_CI.depthBiasClamp = 0.0f;        // Optional
		pp.rasterization_state_CI.depthBiasSlopeFactor = 0.0f;        // Optional
	}

	void GFXPipelineCIBuilder::ConstructDepthStencilStateCI(GFXPipelineCI& pp)
	{
		pp.depth_stencil_CI.depthTestEnable = vk::False;
		pp.depth_stencil_CI.depthWriteEnable = vk::False;

		pp.depth_stencil_CI.depthCompareOp = vk::CompareOp::eLessOrEqual;


		pp.depth_stencil_CI.depthBoundsTestEnable = vk::False;
		//pp.depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
		//pp.depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
		pp.depth_stencil_CI.stencilTestEnable = vk::False;;

		//front and back control the parameters of the stencil test.
		//pp.front = {}; // Optional
		//pp.back = {}; // Optional

	}

	void GFXPipelineCIBuilder::ConstructMultisampleStateCI(GFXPipelineCI& pp)
	{
		pp.multisample_state_CI.sampleShadingEnable = vk::False;
		pp.multisample_state_CI.rasterizationSamples = vk::SampleCountFlagBits::e1;
		pp.multisample_state_CI.minSampleShading = 1.0f;            // Optional
		pp.multisample_state_CI.pSampleMask = nullptr;         // Optional
		pp.multisample_state_CI.alphaToCoverageEnable = vk::False;        // Optional
		pp.multisample_state_CI.alphaToOneEnable = vk::False;        // Optional
	}



	void GFXPipelineCIBuilder::ConstructDynamicState(GFXPipelineCI& pp)
	{



	}

	void GFXPipelineCIBuilder::ConstructColorBlendStateCI(GFXPipelineCI& pp)
	{
		vk::PipelineColorBlendAttachmentState default_color_blend_attachment{};

		default_color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

		default_color_blend_attachment.blendEnable = vk::False;
		default_color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eOne;         // Optional
		default_color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eZero;        // Optional

		default_color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;             // Optional
		default_color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;         // Optional
		default_color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;        // Optional
		default_color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;             // Optional

		pp.color_blend_attachments.push_back(default_color_blend_attachment);



		pp.color_blend_state_CI.logicOpEnable = vk::False;

		pp.color_blend_state_CI.logicOp = vk::LogicOp::eCopy;        // Optional
		pp.color_blend_state_CI.setAttachments(pp.color_blend_attachments);

		pp.color_blend_state_CI.blendConstants[0] = 0.0f;        // Optional
		pp.color_blend_state_CI.blendConstants[1] = 0.0f;        // Optional
		pp.color_blend_state_CI.blendConstants[2] = 0.0f;        // Optional
		pp.color_blend_state_CI.blendConstants[3] = 0.0f;        // Optional
	}

	void GFXPipelineCIBuilder::ConstructVertexInputStateCI(GFXPipelineCI& pp)
	{
	}

	void GFXPipelineCIBuilder::ConstructViewPortStateCI(GFXPipelineCI& pp)
	{
		vk::Viewport       viewport{};
		const vk::Extent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(extend_of_swap_image.height);
		viewport.width = static_cast<float>(extend_of_swap_image.width);
		viewport.height = -static_cast<float>(extend_of_swap_image.height);        //viewport flipping
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Extent2D swap_chain_extent;
		swap_chain_extent.height = extend_of_swap_image.height;
		swap_chain_extent.width = extend_of_swap_image.width;

		vk::Rect2D scissor{};
		scissor.offset = vk::Offset2D{ 0, 0 };
		scissor.extent = swap_chain_extent;

		pp.view_port_scissor_pair.first.push_back(viewport);
		pp.view_port_scissor_pair.second.push_back(scissor);

		pp.viewport_state_CI.viewportCount = static_cast<uint32_t>(pp.view_port_scissor_pair.first.size());
		pp.viewport_state_CI.pViewports = pp.view_port_scissor_pair.first.data();

		pp.viewport_state_CI.scissorCount = static_cast<uint32_t>(pp.view_port_scissor_pair.second.size());
		pp.viewport_state_CI.pScissors = pp.view_port_scissor_pair.second.data();
	}

	void GFXPipelineCIBuilder::ConstructRenderingCI(GFXPipelineCI& pp)
	{
		pp.pipeline_rendering_CI.pNext = nullptr;
	}

}

