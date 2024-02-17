#pragma once

#include "AnniMath.h"
#include "IPassNode.h"
#include "GFXPipelineCIBuilder.h"
#include "GLTFModelFactory.h"
//#include "RenderingMetaInfo.h"

namespace Anni::RenderGraphV1
{

	class PresentPass final : public GraphicsPassNode
	{

		void UpdateDescriptorSets() override
		{

		}



		void CreateDescriptorSetLayout()override
		{

		}

		void GetShaders() override
		{
		}

		void EndRenderPass(VkCommandBuffer cmd_buf)override
		{
		}

		void BeginRenderPass(VkCommandBuffer cmd_buf) override
		{
		}


		void RecordCommandBuffer(const VkCommandBuffer command_buffer) override
		{

		}


		void AllocateDescriptorSets() override
		{
		}




	};

	class DeferedGeometryPass final : public GraphicsPassNode
	{

		DeferedGeometryPass(
			std::string name_,
			DeviceManager& device_manager_,
			SwapchainManager& swapchain_manager_,
			DescriptorLayoutManager& descriptor_set_layout_manager_,
			VkShaderFactory& shader_fac_,
			DescriptorAllocatorGrowable& descriptor_allocator_,
			GFXPipelineCIBuilder& gfx_pipelineCI_builder_,
			std::unordered_map<std::string, VirtualBuffer>& rg_buffers_map_,
			std::unordered_map<std::string, VirtualTexture>& rg_textures_map_
		) :
			GraphicsPassNode(name_, device_manager_, swapchain_manager_, descriptor_set_layout_manager_, shader_fac_, descriptor_allocator_, rg_buffers_map_, rg_textures_map_), gfx_pipelineCI_builder(gfx_pipelineCI_builder_)
		{

		}


	public:
		std::unique_ptr<DescriptorSetLayoutWrapper> common_layout;
		VkDescriptorSet                             common_descriptor_set;

	public:
		std::shared_ptr<VkShaderWrapper> defefered_geometry_vert_shader;
		std::shared_ptr<VkShaderWrapper> defefered_geometry_frag_shader;
	public:
		std::unique_ptr<GFXPipelineCI> gfx_pip_CI;
		LoadedGLTF sponza;
	public:
		GFXPipelineCIBuilder& gfx_pipelineCI_builder;


		void UpdateDescriptorSets() override
		{
			std::vector<VkWriteDescriptorSet> all_writes;
			for (auto& buf_usage : buf_usages)
			{
				if (Vk::RsrcType::Buffer == buf_usage.usage.GetRsrcType())
				{
					all_writes.push_back(buf_usage.v_rsrc->second.p_rsrc->GetWriteDescriptorSetInfo(buf_usage.usage, common_descriptor_set));
				}
			}

			for (auto& tex_usage : tex_usages)
			{
				if (std::holds_alternative<TexUsage>(tex_usage.usage))
				{
					TexUsage& texture_usage = std::get<TexUsage>(tex_usage.usage); // Only works if v holds a TexUsage
					if (Vk::RsrcType::Texture == texture_usage.GetRsrcType())
					{
						all_writes.push_back(tex_usage.v_rsrc->second.p_rsrc->GetWriteDescriptorSetInfo(texture_usage, common_descriptor_set));
					}
				}
			}
			vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), all_writes.size(), all_writes.data(), 0, VK_NULL_HANDLE);
		}



		void CreateDescriptorSetLayout()override
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			for (const auto& buf_usage : buf_usages)
			{
				if (Vk::RsrcType::Buffer == buf_usage.usage.GetRsrcType())
				{
					const Vk::DescriptorInfo desc_info = buf_usage.usage.desc_info;
					const auto one_binding{ DescVk::GetDescriptorSetLayoutBinding(desc_info.slot_info.binding, desc_info.descriptor_type, desc_info.desc_shader_stages_flags) };
					bindings.push_back(one_binding);
				}
			}

			for (auto& tex_usage : tex_usages)
			{
				if (std::holds_alternative<TexUsage>(tex_usage.usage))
				{
					TexUsage& texture_usage = std::get<TexUsage>(tex_usage.usage); // Only works if v holds a TexUsage
					if (Vk::RsrcType::Texture == texture_usage.GetRsrcType())
					{
						const Vk::DescriptorInfo desc_info = texture_usage.desc_info;
						const auto one_binding{ DescVk::GetDescriptorSetLayoutBinding(desc_info.slot_info.binding, desc_info.descriptor_type, desc_info.desc_shader_stages_flags) };
						bindings.push_back(one_binding);
					}
				}
			}

			const auto desc_set_layout_CI = CI::GetDescriptorSetLayoutCI(bindings);
			common_layout = descriptor_set_layout_manager.ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
		}


		void GetShaders() override
		{
			//these shaders can be cached in std map
			defefered_geometry_vert_shader = shader_fac.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
			defefered_geometry_frag_shader = shader_fac.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
		}



		void EndRenderPass(VkCommandBuffer cmd_buf)override
		{
			vkCmdEndRendering(cmd_buf);
		}


		void ImgViewAndSamplerGeneration(VirtualTexRsrcAndUsage& rsrc_usage)
		{
			std::shared_ptr<VkTexture>& ptr_tex = rsrc_usage.v_rsrc->second.p_rsrc;
			std::variant<TexUsage, AttachUsage>& tex_usage = rsrc_usage.usage;

			std::visit([&](auto& variant_usage)
				{
					if (!variant_usage.img_view_CI)
					{
						variant_usage.img_view = ptr_tex->GetTextureImageViewPtr();
					}
					else
					{
						variant_usage.img_view = std::make_shared<ImgViewWrapper>(device_manager, variant_usage.img_view_CI.value());
					}

					if (!variant_usage.sampler_CI)
					{
						variant_usage.sampler = ptr_tex->GetTextureSamplerPtr();
					}
					else
					{
						variant_usage.sampler = std::make_shared<SamplerWrapper>(device_manager, variant_usage.sampler_CI.value());
					}
				}, tex_usage);
		}


		void BeginRenderPass(VkCommandBuffer cmd_buf) override
		{
			VkRenderingInfo rendering_info{};
			rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			rendering_info.renderArea.offset = { 0, 0 };
			rendering_info.renderArea.extent = swapchain_manager.GetSwapChainImageExtent2D();
			rendering_info.layerCount = 1;
			rendering_info.pNext = nullptr;
			rendering_info.flags = 0;

			std::vector<VkRenderingAttachmentInfo>   color_attachment_infos;
			VkRenderingAttachmentInfo                depth_attachment_info;
			VkRenderingAttachmentInfo                stensil_attachment_info;


			//先把sampler 和 view 创建好。
			for (auto& tex_usage : tex_usages)
			{
				if (std::holds_alternative<AttachUsage>(tex_usage.usage))
				{
					ImgViewAndSamplerGeneration(tex_usage);
				}
			}


			for (auto& tex_usage : tex_usages)
			{
				if (std::holds_alternative<AttachUsage>(tex_usage.usage))
				{
					tex_usage.BindResolveTarget();
					AttachUsage& attach_usage = std::get<AttachUsage>(tex_usage.usage); // Only works if v holds an AttachUsage
					const auto attach_type = attach_usage.attach_info.attach_type;
					if (Vk::AttachmentType::ColorAttachment == attach_type)
					{
						color_attachment_infos.push_back(attach_usage.GetVkRenderingAttachmentInfo());
					}
					if (Vk::AttachmentType::DepthAttachment == attach_type ||
						Vk::AttachmentType::DepthStencilAttachment == attach_type)
					{
						depth_attachment_info = attach_usage.GetVkRenderingAttachmentInfo();
					}
				}
			}
			rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
			rendering_info.pColorAttachments = color_attachment_infos.data();
			rendering_info.pDepthAttachment = &depth_attachment_info;
			rendering_info.pStencilAttachment = VK_NULL_HANDLE;        //这个pStencilAttachment是reserve for future use的
			vkCmdBeginRendering(cmd_buf, &rendering_info);
		}



		void RecordCommandBuffer(const VkCommandBuffer command_buffer) override
		{
			//SHADERS
			const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{
				defefered_geometry_vert_shader->GetShaderStageCI(),
				defefered_geometry_frag_shader->GetShaderStageCI()
			};




			//DYNAMIC RENDERING FORMATS
			std::vector<DynamicRenderingAttachmentFormatInfo> dynamic_rendering_attach_formats;
			for (auto& tex_usage : tex_usages)
			{
				if (std::holds_alternative<AttachUsage>(tex_usage.usage))
				{
					AttachUsage& attach_usage = std::get<AttachUsage>(tex_usage.usage); // Only works if v holds an AttachUsage
					dynamic_rendering_attach_formats.push_back(attach_usage.GetDynamicRenderingAttachmentFormatInfo());
				}
			}

			gfx_pip_CI = gfx_pipelineCI_builder.GetPipelineCI();
			//for reversed Z value
			gfx_pip_CI->depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
			//multisample
			gfx_pip_CI->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
			gfx_pip_CI->multisample_state_CI.sampleShadingEnable = VK_FALSE;

			gfx_pip_CI->SetDynamicRenderingAttachmentFormats(dynamic_rendering_attach_formats);
			gfx_pip_CI->SetPipelineShaderStageCreateInfo(shader_stages);
			gfx_pip_CI->AddDescriptorSetLayout(common_layout->GetRawDescriptorSetLayout());


			DrawContext draw_context;
			//Model Drawing(Sponza)
			sponza.BuildPipeline(*gfx_pip_CI);

			{

				sponza.Draw(glm::mat4(1.0f), draw_context);
				for (const auto& r : draw_context.OpaqueSurfaces)
				{
					auto surface_pipeline_layout = r.material->pipeline->pipeline_layout->GetRaw();
					auto surface_pipeline = r.material->pipeline->pipeline->GetRaw();

					vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, surface_pipeline);
					//Bind common descriptor set(not related to model) 
					vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, surface_pipeline_layout, 0, 1,
						&common_descriptor_set, 0, nullptr);
					vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, surface_pipeline_layout, 1, 1,
						&r.material->materialSet, 0, nullptr);
					vkCmdBindIndexBuffer(command_buffer, r.indexBuffer->GetGPUBuffer(), 0, VK_INDEX_TYPE_UINT32);
					// calculate final mesh matrix
					GPUDrawPushConstants push_constants;
					push_constants.worldMatrix = r.transform;
					push_constants.vertexBuffer = r.vertexBufferAddress;
					vkCmdPushConstants(command_buffer, surface_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
					vkCmdDrawIndexed(command_buffer, r.indexCount, 1, r.firstIndex, 0, 0);
				}
			}
		}

		void AllocateDescriptorSets() override
		{
			common_descriptor_set = descriptor_allocator.Allocate(common_layout->GetRawDescriptorSetLayout());
		}

	};

}


