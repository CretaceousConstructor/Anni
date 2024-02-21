#include "RealtimeRenderer.h"

namespace Anni::Renderer
{
	void
	RealtimeRenderer::CreateGlobalModels()
	{
		const std::string sponza_path = "Asset/Models/Sponza/glTF/Sponza.gltf";
		test_model_sponza = gltf_model_fac.LoadGLTF(sponza_path);

		// const std::string scifi_helmet_path =
		// "../../data/models/SciFiHelmet/SciFiHelmet.gltf";
		// global_resources.scifi_helmet       =
		// model_factory.GetGltfModel<PbrMaterialMetallic>(scifi_helmet_path,
		// VkModelFactory::LoadingOption::None, 0);

		// const std::string box_path = "../../data/models/cube/Cube.gltf";
		// global_resources.sky_box   =
		// model_factory.GetGltfModel<PbrMaterialMetallic>(box_path,
		// VkModelFactory::LoadingOption::None, 1);

		// const std::string sponza_path = "../../data/models/sponza/sponza.gltf";
		// persistent_resources.sponza = model_factory.GetGltfModel<NonPbrMat>(
		//	sponza_path, Vk::ModelLoadingOption::None);

		// auto                  data = Geometry::CreateSphere(.2f, 20, 40,
		// glm::vec4(1.f, 1.f, 1.f, 1.f)); std::vector<Vertex>   vertices;

		// std::vector<uint32_t> indices;
		// for (auto &v : data.vertexVec)
		//{
		//	vertices.push_back(Vertex(v.pos));
		// }
		// indices = std::move(data.indexVec);

		// light_indicator = std::make_unique<VkModel<Vertex>>(vertices,indices,
		// device_manager, window->GetSurfaceRef(), transfer_command_buffer);
		// light_indicator->GetTransform().SetPosition(ubo_vs_scene.light_pos);
	}

	// void RealtimeRenderer::CommandBufferRecording()
	//{
	//	// auto &graphics_command_buffers =
	//	// command_manager.GetGraphicsCommandBuffers();

	//	// VkCommandManager::BeginCommandBuffers(graphics_command_buffers);

	//	// for (size_t i = 0; i < renderpasses.size(); i++)
	//	//{
	//	//	renderpasses[i]->ExecuteStatically(graphics_command_buffers);
	//	// }

	//	// VkCommandManager::EndCommandBuffers(graphics_command_buffers);
	//}

	void
	RealtimeRenderer::CreateGlobalBuffers()
	{
		// GPU SIDE
		// FLLOWING SHIT IS USED BY PBR
		//{
		//	constexpr VkBufferCI::UniformBuffer unim_buf_PP;
		//	global_resources.matrix_buffer_gpu =
		// buffer_factory.ProduceBufferPtrArray(sizeof(Global::Structure::UboMatrix),
		// Vk::BundleSize<SWAP_IMG_COUNT>, unim_buf_PP);
		// }

		// GPU SIDE
		//{
		//	constexpr VkBufferCI::UniformBuffer unim_buf_PP;
		//	global_resources.matrix_buffer_gpu_MSAA =
		// buffer_factory.ProduceBufferPtrArray(sizeof(MSAA::UBO),
		// Vk::BundleSize<SWAP_IMG_COUNT>, unim_buf_PP);
		// }

		// GPU SIDE
		{
			// for (const auto& [index, frame_data] :
			//	(frame_datas | std::views::enumerate))
			//{
			//	frame_data.uniform_buffer_gpu_MSAA = buf_fac.ProduceBufferReFac(
			//		sizeof(MSAA::UBO), CI::UniformBuffer);
			// }

			for (auto& frame_data : frame_datas)
			{
				frame_data.uniform_buffer_gpu_MSAA = buf_fac.ProduceBuffer(sizeof(MSAA::UBO), CI::UniformBuffer);
			}
		}
	}

	// void RealtimeRenderer::CreateCommonDescriptorPool()
	//{
	//	// Create the global descriptor pool
	//	// TODO:这里的分配数目

	//	// auto &des_man = renderpass_manager.GetDescriptorManager();
	//	//{
	//	//	std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs{
	//	//	    std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3}};
	//	//	const auto max_sets = swapchain_manager.GetSwapImageCount();
	//	//	des_man.AddDescriptorPool(Global::pool_main_thread,
	//	// std::move(info_pairs), max_sets);
	//	// }

	//	//	const auto max_sets = swapchain_manager.GetSwapImageCount();

	//	// const std::vector desc_pool_sizes{
	//	//	Vk::GetOneDescriptorPoolSizeDescription(
	//	//		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//	//		Vk::DescriptorCount<Vk::SWAP_IMG_COUNT> *20),
	//	//	Vk::GetOneDescriptorPoolSizeDescription(
	//	//		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	//	//		Vk::DescriptorCount<Vk::SWAP_IMG_COUNT> *20),
	//	// };

	//	// const auto desc_pool_CI = Vk::GetDescriptorPoolCI(
	//	//	desc_pool_sizes, Vk::SetCount<Vk::SWAP_IMG_COUNT>);
	//	// persistent_resources.pool =
	//	//
	//renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(
	//	//		desc_pool_CI);
	//}

	void
	RealtimeRenderer::InitRenderpasses()
	{
		// use factory mode to optimize
		//
		// renderpasses.push_back(std::make_shared<PrefilterAndLUTMapGenPass>(gfx,
		//  renderpass_manager, global_resources)); renderpasses.back()->Init();

		// renderpasses.push_back(std::make_shared<IrradianceMapGenPass>(gfx,
		// renderpass_manager, global_resources)); renderpasses.back()->Init();

		// renderpasses.push_back(std::make_shared<PbrRenderingPass>(gfx,
		// renderpass_manager, global_resources)); renderpasses.back()->Init();

		// VkRenderpassBase::ProduceRenderpass<DeferedGeometryPass>(gfx,
		// renderpass_manager, global_resources);

		// renderpasses.push_back(std::make_shared<DeferedGeometryPass>(gfx,
		// renderpass_manager, global_resources)); renderpasses.back()->Init();

		//
		// renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<DeferedGeometryPass>(gfx,
		//  renderpass_manager, persistent_resources));
		//
		// renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<DeferedCompositionPass>(gfx,
		//  renderpass_manager, persistent_resources));

		//
		// renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<MSAAPass>(gfx,
		//  renderpass_manager, global_resources));

		// TWO PHRASES INIT:use template to eliminate this.
		// renderpasses.push_back(std::make_shared<DeferedGeometryPass>(gfx,
		// renderpass_manager, persistent_resources));
		// renderpasses.back()->Init();
		//  renderpasses.push_back(std::make_shared<DeferedCompositionPass>(gfx,
		//  renderpass_manager, persistent_resources));
		// renderpasses.back()->Init();

		// renderpasses_RG.push_back(std::make_shared<DeferedGeometryPassRGV0>(
		// gfx, renderpass_manager, render_graph_v0,
		// persistent_resources));
		// renderpasses_RG.push_back(std::make_shared<DeferedCompositionPassRGV0>(
		// gfx, renderpass_manager, render_graph_v0,
		// persistent_resources));
	}

	void
	RealtimeRenderer::InitSynObjects()
	{
		for (size_t i = 0; i < frame_num_semaphores.size(); ++i)
		{
			frame_num_semaphores[i] = std::make_shared<TimelineSemWrapper>(device_manager);
		}

		for (size_t i = 0; i < Vk::MAX_FRAMES_OVERLAP; ++i)
		{
			present_finished_semaphores.push_back(std::make_shared<BinarySemWrapper>(device_manager));
		}


		for (size_t i = 0; i < Vk::MAX_FRAMES_OVERLAP; ++i)
		{
			ready_present_semaphores.emplace_back(std::make_shared<BinarySemWrapper>(device_manager));
		}


		for (size_t i = 0; i < swapchain_manager.GetSwapImageCount(); ++i)
		{
			swap_img_rdy_4_rendering_helper_sem.push_back(std::make_shared<TimelineSemWrapper>(device_manager));
		}

		for (size_t i = 0; i < swapchain_manager.GetSwapImageCount(); ++i)
		{
			present_finished_helper_sem.push_back(std::make_shared<TimelineSemWrapper>(device_manager));
		}
	}


	void
	RealtimeRenderer::UpdateUniformBuffer(size_t current_image_index)
	{
		// struct UBO_VS_SCENE{
		//	glm::mat4 projection;
		//	glm::mat4 view;
		//	glm::vec4 light_pos   = glm::vec4(0.0f, 2.5f, 0.0f, 1.0f);
		//	glm::vec4 view_pos;
		// } ubo_vs_scene;                   //用于顶点着色器的uniform buffer object

		////FLLOWING SHIT IS USED BY PBR
		// global_resources.matrix_buffer_cpu.projection = camera->GetProjMatrix();
		// global_resources.matrix_buffer_cpu.view = camera->GetViewMatrix();
		// global_resources.matrix_buffer_cpu.cam_pos = camera->GetEyePos();
		//
		// global_resources.matrix_buffer_gpu[current_image_index]->CopyFrom(&global_resources.matrix_buffer_cpu,
		// sizeof(global_resources.matrix_buffer_cpu));

		// static bool first_time = true;
		// if (first_time)
		//{
		//	/*******************************cpu side ubuffer
		//	 * init**********************************/
		//	 // Lights
		//	 // White
		//	matrix_buffer_cpu_defered_rendering.lights[0].position =
		//		glm::vec4(1.0f, .7f, 0.f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[0].color = glm::vec3(1.5f);
		//	matrix_buffer_cpu_defered_rendering.lights[0].radius = 15.0f * 0.25f;

		//	// Red
		//	matrix_buffer_cpu_defered_rendering.lights[1].position =
		//		glm::vec4(-1.0f, .7f, 0.f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[1].color =
		//		glm::vec3(1.0f, 0.0f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[1].radius = 15.0f;
		//	// Blue
		//	matrix_buffer_cpu_defered_rendering.lights[2].position =
		//		glm::vec4(5.5f, .7f, 0.f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[2].color =
		//		glm::vec3(0.0f, 0.0f, 2.5f);
		//	matrix_buffer_cpu_defered_rendering.lights[2].radius = 5.0f;
		//	// Yellow
		//	matrix_buffer_cpu_defered_rendering.lights[3].position =
		//		glm::vec4(-5.5f, .7f, 0.f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[3].color =
		//		glm::vec3(1.0f, 1.0f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[3].radius = 2.0f;
		//	// Green
		//	matrix_buffer_cpu_defered_rendering.lights[4].position =
		//		glm::vec4(9.5f, .7f, 0.f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[4].color =
		//		glm::vec3(0.0f, 1.0f, 0.2f);
		//	matrix_buffer_cpu_defered_rendering.lights[4].radius = 5.0f;
		//	// Yellow
		//	matrix_buffer_cpu_defered_rendering.lights[5].position =
		//		glm::vec4(-9.5f, .7f, 0.f, 0.0f);
		//	matrix_buffer_cpu_defered_rendering.lights[5].color =
		//		glm::vec3(1.0f, 0.7f, 0.3f);
		//	matrix_buffer_cpu_defered_rendering.lights[5].radius = 25.0f;
		//}

		// matrix_buffer_cpu_defered_rendering.projection = camera->GetProjMatrix(
		//	Camera::ProjectionMtxSetting::ReversedZ);  // for revered Z
		// matrix_buffer_cpu_defered_rendering.view = camera->GetViewMatrix();
		// matrix_buffer_cpu_defered_rendering.view_inverse =
		//	camera->GetInverseViewMatrix();
		// matrix_buffer_cpu_defered_rendering.cam_pos = camera->GetEyePos();

		// COPY
		// frame_datas[current_image_index].uniform_buffer_gpu_MSAA->CopyFromHost(
		//	&matrix_buffer_cpu_defered_rendering,
		//	sizeof(matrix_buffer_cpu_defered_rendering));

		frame_datas[current_image_index].matrix_buffer_cpu_MSAA.projection = camera->GetProjMatrix(
			Camera::ProjectionMtxSetting::ReversedZ); // for revered Z
		frame_datas[current_image_index].matrix_buffer_cpu_MSAA.view = camera->GetViewMatrix();
		frame_datas[current_image_index].matrix_buffer_cpu_MSAA.view_inverse = camera->GetInverseViewMatrix();
		frame_datas[current_image_index].matrix_buffer_cpu_MSAA.cam_pos = camera->GetEyePos();

		// COPY
		frame_datas[current_image_index].uniform_buffer_gpu_MSAA->CopyFromHost(
			&frame_datas[current_image_index].matrix_buffer_cpu_MSAA,
			sizeof(frame_datas[current_image_index].matrix_buffer_cpu_MSAA));
	}


	void
	RealtimeRenderer::UpdateCamera(float dt)
	{
		// TODO:用命令模式优化
		static bool stop_cam = false;

		if (keyboard->GetIsKeyDown(GLFW_KEY_G))
		{
			stop_cam = (!stop_cam);
		}

		if (stop_cam)
		{
			return;
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(const_cast<GLFWwindow*>(window.GetWindowPtr()),
			                         GLFW_TRUE);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_W))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Forward, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_S))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Backward, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_A))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Left, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_D))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Right, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_Q))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::RollLeft, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_E))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::RollRight, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_Z))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::ZoomIn, dt);
		}

		if (keyboard->GetIsKeyDown(GLFW_KEY_C))
		{
			camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::ZoomOut, dt);
		}

		camera->ProcessMouseMovement(mouse->GetYawDiff(), mouse->GetPitchDiff());
		camera->ProcessMouseScroll(mouse->GetMouseScroll());
	}

	// RealtimeRenderer::RealtimeRenderer(GraphicsComponent& gfx_)
	//	: IRenderer(gfx_),
	//	renderpass_manager(gfx),
	//	imgui_UI(gfx),
	//	render_graph_v0(device_manager, renderpass_manager)
	//	//,render_graph(renderpass_manager)
	//{
	// }

	void
	RealtimeRenderer::SetUpUserInput()
	{
		std::vector<int> tracked_keys = {
			GLFW_KEY_ESCAPE, GLFW_KEY_W, GLFW_KEY_S,
			GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q,
			GLFW_KEY_E, GLFW_KEY_G, GLFW_KEY_UP,
			GLFW_KEY_DOWN
		};
		keyboard = std::make_unique<KeyBoardInputManager>(tracked_keys);
		keyboard->SetupKeyInputs(window.GetWindowPtr());

		mouse = std::make_unique<MouseInputManager>(gfx);
	}

	void
	RealtimeRenderer::CreateCamera()
	{
		camera = std::make_unique<FirstPersonCamera>();
		camera->SetFrustum(
			glm::radians(60.f),
			static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width) /
			static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height),
			0.1f,
			256.f);
	}

	void
	RealtimeRenderer::CreateGlobalAttachments()
	{
		CreateSwapchainTextures();
		//CreateDepthTextures();
	}

	//

	void
	RealtimeRenderer::CreateGlobalTextures()
	{
		// constexpr auto chiricahua_narrowPath_path =
		// "../../data/textures/hdr/NarrowPath_3k.hdr"; const auto
		// sampler_CI = CI::PopulateTexSamplerCI();

		// tex_fac.ProduceTextureFromImgPath(chiricahua_narrowPath_path,
		// VK_FORMAT_R32G32B32A32_SFLOAT, sampler_CI,std::nullopt);
	}

	//// the creation of this resources should be taken care of by render graph,
	/// which / haven't been implemented.

	void
	RealtimeRenderer::CreateSwapchainTextures()
	{
		const auto swap_img_CI = CI::GetSwapchainImgCI();
		const auto img_view_CI = CI::PopulateSwapchainImgViewCI(swapchain_manager);
		for (const auto& [index, frame_data] : (frame_datas | std::views::enumerate))
		{
			frame_data.swapchain_attachment = tex_fac.ProduceSwapTexture(swap_img_CI, std::nullopt, img_view_CI, index);
			frame_datas[index].swapchain_attachment->swap_img_rdy_4_rendering_helper_sem =
				swap_img_rdy_4_rendering_helper_sem[index];
			frame_datas[index].swapchain_attachment->present_finished_helper_sem = present_finished_helper_sem[index];
		}
	}

	//void
	//	RealtimeRenderer::CreateDepthTextures()
	//{
	//	// Use depth stencil format, cuz we will do stencil writing.
	//	// auto depth_img_CI = CI::GetDepthImgCI(swapchain_manager);
	//	// auto img_view_CI = CI::PopulateDepthImgViewCI(gfx.SwapchainMan());

	//	// for (auto& frame_data : frame_datas)
	//	//{
	//	//	frame_data.depth_attachment = tex_fac.ProduceUnfilledTextureReFac(
	//	//		depth_img_CI, std::nullopt, img_view_CI);
	//	// }
	//}

	void RealtimeRenderer::ImportModelRsrcToRenderGraph(
		Anni::LoadedGLTF& model,
		Anni::RenderGraphV1::GraphicsPassNode& pass_node)
	{
		// These model loading shit can be acheived by function
		for (const auto& [_, mesh] : model.meshes)
		{
			// TODO:这些名字都可以自动生成
			pass_node.In(mesh->name + "IndexBuffer",
			             mesh->meshBuffers.indexBuffer,
			             Anni::RenderGraphV1::BufUsage{
				             Anni::RsrcType::IndexBuffer,
				             Anni::BufSyncInfo{
					             .access_mask = vk::AccessFlagBits2::eIndexRead,
					             .stage_mask = vk::PipelineStageFlagBits2::eIndexInput,
				             },
				             Anni::RsrcAccessTypeRG::Read
			             });

			pass_node.In(mesh->name + "VertexBuffer",
			             mesh->meshBuffers.vertexBuffer,
			             Anni::RenderGraphV1::BufUsage{
				             Anni::RsrcType::AddressRefedBuffer,
				             Anni::BufSyncInfo{
					             .access_mask = vk::AccessFlagBits2::eShaderRead,
					             .stage_mask = vk::PipelineStageFlagBits2::eVertexShader
				             },
				             Anni::RsrcAccessTypeRG::Read
			             });
		}

		const auto& texures_vsrc_and_usage =
			pass_node.In(model.name + "Textures",
			             model.images_array,
			             Anni::RenderGraphV1::TexUsage(
				             Anni::ImgSyncInfo{
					             .access_mask = vk::AccessFlagBits2::eShaderRead,
					             .stage_mask =
					             vk::PipelineStageFlagBits2::eFragmentShader,
					             .layout_inpass =
					             vk::ImageLayout::eShaderReadOnlyOptimal
				             }));

		pass_node.In(model.name + "UniformBuffer",
		             model.materialDataBuffer,
		             Anni::RenderGraphV1::BufUsage{
			             Anni::RsrcType::ModelBuffer,
			             Anni::BufSyncInfo{
				             .access_mask = vk::AccessFlagBits2::eShaderRead,
				             .stage_mask = vk::PipelineStageFlagBits2::eFragmentShader
			             },
			             Anni::RsrcAccessTypeRG::Read,
		             });


		pass_node.model_to_multi_tex_usage.emplace(
			&model, std::get<Anni::RenderGraphV1::TexUsage>(texures_vsrc_and_usage.usage));
	}

	void
	RealtimeRenderer::RenderGraphRendering(uint32_t img_index, uint32_t cur_frame)
	{
		render_graph_v1.SetCurFrameAndImgIndex(cur_frame, img_index);
		auto& DeferedGeoPass =
			render_graph_v1.AddGfxPassNode<RenderGraphV1::DeferedGeometryPass>("DeferedGeometryPass");
		ImportModelRsrcToRenderGraph(*test_model_sponza, DeferedGeoPass);

		DeferedGeoPass.In(
			std::string("SceneData"),
			frame_datas[img_index].uniform_buffer_gpu_MSAA,
			Anni::RenderGraphV1::BufUsage(
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<0>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eUniformBuffer,
					vk::ShaderStageFlagBits::eVertex |
					vk::ShaderStageFlagBits::eFragment
				},
				Anni::BufSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask =
					vk::PipelineStageFlagBits2::eVertexShader |
					vk::PipelineStageFlagBits2::eFragmentShader
				},
				Anni::RsrcAccessTypeRG::Read));

		auto& gbuf_pos_outlet =
			DeferedGeoPass.Out(
				std::string("GBufPos"),
				VkTexture::Descriptor{
					.tex_img_CI =
					CI::GetTextureImgCI(DeferedRendering::G_position_format,
					                    swapchain_manager.GetSwapChainImageExtent()),
					.img_view_CI =
					CI::PopulateTextureImgViewCI(DeferedRendering::G_position_format),
					.sampler_CI = std::nullopt,
				},

				[](VkTexture::Descriptor& desco)
				{
					// descriptor就是创建参数，虚拟资源只有在execute执行的时候才会创建真正的资源
					desco.tex_img_CI.vk_image_CI.usage =
						vk::ImageUsageFlagBits::eColorAttachment |
						vk::ImageUsageFlagBits::eSampled;
					desco.tex_img_CI.vk_image_CI.samples =
						DeferedRendering::MSAA_sample_count;
				},

				Anni::RenderGraphV1::AttachUsage(
					DeferedRendering::G_position_format,
					Anni::AttachmentInfo{
						.attachment_index = Vk::AttachmentIndex<0>,
						.attach_type = Anni::AttachmentType::ColorAttachment,
						.clear_value = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f},
					},
					Anni::ImgSyncInfo{
						.access_mask = vk::AccessFlagBits2::eColorAttachmentWrite,
						.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
						.layout_inpass = vk::ImageLayout::eColorAttachmentOptimal
					},
					Anni::RsrcAccessTypeRG::Write));

		auto& gbuf_normal_outlet =
			DeferedGeoPass.Out(
				std::string("GBufNormal"),
				VkTexture::Descriptor{
					.tex_img_CI =
					CI::GetTextureImgCI(DeferedRendering::G_normal_format,
					                    swapchain_manager.GetSwapChainImageExtent()),
					.img_view_CI =
					CI::PopulateTextureImgViewCI(DeferedRendering::G_normal_format),
					.sampler_CI = std::nullopt,
				},
				[](VkTexture::Descriptor& desco)
				{
					desco.tex_img_CI.vk_image_CI.usage =
						vk::ImageUsageFlagBits::eColorAttachment |
						vk::ImageUsageFlagBits::eSampled;
					desco.tex_img_CI.vk_image_CI.samples =
						DeferedRendering::MSAA_sample_count;
				},
				Anni::RenderGraphV1::AttachUsage(
					DeferedRendering::G_normal_format,
					Anni::AttachmentInfo{
						.attachment_index = Vk::AttachmentIndex<1>,
						.attach_type = Anni::AttachmentType::ColorAttachment,
						.clear_value = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}
					},
					Anni::ImgSyncInfo{
						.access_mask = vk::AccessFlagBits2::eColorAttachmentWrite,
						.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
						.layout_inpass = vk::ImageLayout::eColorAttachmentOptimal
					},
					Anni::RsrcAccessTypeRG::Write));

		auto& gbuf_albedo_outlet =
			DeferedGeoPass.Out(
				std::string("GBufAlbedo"),
				VkTexture::Descriptor{
					.tex_img_CI =
					CI::GetTextureImgCI(DeferedRendering::G_albedo_format,
					                    swapchain_manager.GetSwapChainImageExtent()),
					.img_view_CI =
					CI::PopulateTextureImgViewCI(DeferedRendering::G_albedo_format),
					.sampler_CI = std::nullopt,
				},
				[](VkTexture::Descriptor& desco)
				{
					// descriptor就是创建参数，虚拟资源只有在execute执行的时候才会创建真正的资源
					desco.tex_img_CI.vk_image_CI.usage =
						vk::ImageUsageFlagBits::eColorAttachment |
						vk::ImageUsageFlagBits::eSampled;
					desco.tex_img_CI.vk_image_CI.samples =
						DeferedRendering::MSAA_sample_count;
				},

				Anni::RenderGraphV1::AttachUsage(
					DeferedRendering::G_albedo_format,
					Anni::AttachmentInfo{
						.attachment_index = Vk::AttachmentIndex<2>,
						.attach_type = Anni::AttachmentType::ColorAttachment,
						.clear_value = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}
					},
					Anni::ImgSyncInfo{
						.access_mask = vk::AccessFlagBits2::eColorAttachmentWrite,
						.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
						.layout_inpass = vk::ImageLayout::eColorAttachmentOptimal
					},
					Anni::RsrcAccessTypeRG::Write));

		auto& gbuf_posZGradient_outlet =
			DeferedGeoPass.Out(
				"GBufPosZGradient",
				VkTexture::Descriptor{
					.tex_img_CI = CI::GetTextureImgCI(
						DeferedRendering::G_posZgrad_format,
						swapchain_manager.GetSwapChainImageExtent()),
					.img_view_CI = CI::PopulateTextureImgViewCI(
						DeferedRendering::G_posZgrad_format),
					.sampler_CI = std::nullopt
				},
				[](VkTexture::Descriptor& desco)
				{
					// descriptor就是创建参数，虚拟资源只有在execute执行的时候才会创建真正的资源
					desco.tex_img_CI.vk_image_CI.usage =
						vk::ImageUsageFlagBits::eColorAttachment |
						vk::ImageUsageFlagBits::eSampled;
					desco.tex_img_CI.vk_image_CI.samples =
						DeferedRendering::MSAA_sample_count;
				},
				Anni::RenderGraphV1::AttachUsage(
					DeferedRendering::G_posZgrad_format,

					Anni::AttachmentInfo{
						.attachment_index = Vk::AttachmentIndex<3>,
						.attach_type = Anni::AttachmentType::ColorAttachment,
						.clear_value = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}
					},
					Anni::ImgSyncInfo{
						.access_mask = vk::AccessFlagBits2::eColorAttachmentWrite,
						.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
						.layout_inpass = vk::ImageLayout::eColorAttachmentOptimal
					},

					Anni::RsrcAccessTypeRG::Write));

		auto& gbuf_depth_outlet =
			DeferedGeoPass.Out(
				"GBufDepth",
				VkTexture::Descriptor{
					.tex_img_CI =
					CI::GetTextureImgCI(DeferedRendering::G_depth_format,
					                    swapchain_manager.GetSwapChainImageExtent()),
					.img_view_CI =
					CI::PopulateTextureImgViewCI(DeferedRendering::G_depth_format),
					.sampler_CI = std::nullopt,
				},
				[](VkTexture::Descriptor& desco)
				{
					desco.tex_img_CI.vk_image_CI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment |
						vk::ImageUsageFlagBits::eSampled;
					desco.tex_img_CI.vk_image_CI.samples = DeferedRendering::MSAA_sample_count;
				},
				Anni::RenderGraphV1::AttachUsage(
					DeferedRendering::G_depth_format,
					Anni::AttachmentInfo{
						.attachment_index = Vk::AttachmentIndex<4>,
						.attach_type = Anni::AttachmentType::DepthAttachment,
						.clear_value = vk::ClearDepthStencilValue{0.0f, 0}
					},
					Anni::ImgSyncInfo{
						.access_mask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
						.stage_mask = vk::PipelineStageFlagBits2::eLateFragmentTests,
						.layout_inpass = vk::ImageLayout::eDepthAttachmentOptimal
					},

					Anni::RsrcAccessTypeRG::Write));

		auto& DeferedComPass =
			render_graph_v1.AddGfxPassNode<RenderGraphV1::DeferedGeometryPass>(
				"DeferedGeometryPass");

		DeferedComPass.In(
			"SceneData",
			frame_datas[img_index].uniform_buffer_gpu_MSAA,
			Anni::RenderGraphV1::BufUsage(
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<0>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eUniformBuffer,
					vk::ShaderStageFlagBits::eVertex |
					vk::ShaderStageFlagBits::eFragment
				},
				Anni::BufSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask =
					vk::PipelineStageFlagBits2::eFragmentShader |
					vk::PipelineStageFlagBits2::eVertexShader
				},
				Anni::RsrcAccessTypeRG::Read));

		DeferedComPass.In(
			gbuf_pos_outlet,
			Anni::RenderGraphV1::TexUsage(
				DeferedRendering::G_position_format,
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<1>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eSampledImage,
					vk::ShaderStageFlagBits::eFragment
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask = vk::PipelineStageFlagBits2::eFragmentShader,
					.layout_inpass = vk::ImageLayout::eShaderReadOnlyOptimal
				},
				Anni::RsrcAccessTypeRG::Read));

		DeferedComPass.In(
			gbuf_normal_outlet,
			Anni::RenderGraphV1::TexUsage(
				DeferedRendering::G_normal_format,
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<2>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eSampledImage,
					vk::ShaderStageFlagBits::eFragment
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask = vk::PipelineStageFlagBits2::eFragmentShader,
					.layout_inpass = vk::ImageLayout::eShaderReadOnlyOptimal
				},
				Anni::RsrcAccessTypeRG::Read));

		DeferedComPass.In(
			gbuf_albedo_outlet,
			Anni::RenderGraphV1::TexUsage(
				DeferedRendering::G_albedo_format,
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<3>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eSampledImage,
					vk::ShaderStageFlagBits::eFragment
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask = vk::PipelineStageFlagBits2::eFragmentShader,
					.layout_inpass = vk::ImageLayout::eShaderReadOnlyOptimal
				},
				Anni::RsrcAccessTypeRG::Read));

		DeferedComPass.In(
			gbuf_posZGradient_outlet,
			Anni::RenderGraphV1::TexUsage(
				DeferedRendering::G_posZgrad_format,
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<4>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eSampledImage,
					vk::ShaderStageFlagBits::eFragment
				},

				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask = vk::PipelineStageFlagBits2::eFragmentShader,
					.layout_inpass = vk::ImageLayout::eShaderReadOnlyOptimal
				},
				Anni::RsrcAccessTypeRG::Read));

		DeferedComPass.In(
			gbuf_depth_outlet,
			Anni::RenderGraphV1::TexUsage(
				DeferedRendering::G_depth_format,
				Anni::DescriptorInfo{
					Anni::DescriptorSetInfo{
						.set = Vk::SetIndex<0>,
						.binding = Vk::Binding<4>,
						.array_element = Vk::BindingArrayElement<0>,
					},
					vk::DescriptorType::eSampledImage,
					vk::ShaderStageFlagBits::eFragment
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eShaderRead,
					.stage_mask = vk::PipelineStageFlagBits2::eFragmentShader,
					.layout_inpass = vk::ImageLayout::eShaderReadOnlyOptimal
				},
				Anni::RsrcAccessTypeRG::Read));

		auto& MSColorAttach = DeferedComPass.Out(
			"MSColorAttach",
			VkTexture::Descriptor{
				.tex_img_CI =
				CI::GetTextureImgCI(swapchain_manager.GetSwapChainImageFormat(),
				                    swapchain_manager.GetSwapChainImageExtent()),
				.img_view_CI =
				CI::PopulateTextureImgViewCI(DeferedRendering::C_color_attch_format),
				.sampler_CI = std::nullopt,
			},
			[](VkTexture::Descriptor& desco)
			{
				desco.tex_img_CI.vk_image_CI.usage =
					vk::ImageUsageFlagBits::eColorAttachment;
				desco.tex_img_CI.vk_image_CI.samples =
					DeferedRendering::MSAA_sample_count;
				// 这个VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT多数在移动端才能被支持，这里如果加这个flag就创建不了
				// tex_img_CI.default_image_mem_prop_flag =
				// VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
			},
			Anni::RenderGraphV1::AttachUsage(
				DeferedRendering::C_color_attch_format,

				Anni::AttachmentInfo{
					.attachment_index = Vk::AttachmentIndex<0>,
					.attach_type = Anni::AttachmentType::ColorAttachment,
					.clear_value = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eColorAttachmentWrite,
					.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,

					.layout_inpass = vk::ImageLayout::eColorAttachmentOptimal
				},
				Anni::RsrcAccessTypeRG::Write));

		auto& MSDepthStencilAttach = DeferedComPass.Out(
			"MSDepthStencilAttach",
			VkTexture::Descriptor{
				.tex_img_CI =
				CI::GetTextureImgCI(DeferedRendering::C_depth_stencil_format,
				                    swapchain_manager.GetSwapChainImageExtent()),
				.img_view_CI =
				CI::PopulateDepthImgViewCI(DeferedRendering::C_depth_stencil_format),
				.sampler_CI = std::nullopt,
			},
			[](VkTexture::Descriptor& desco)
			{
				desco.tex_img_CI.vk_image_CI.usage =
					vk::ImageUsageFlagBits::eDepthStencilAttachment;
				desco.tex_img_CI.vk_image_CI.samples =
					DeferedRendering::MSAA_sample_count;
				// 这个VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT多数在移动端才能被支持，这里如果加这个flag就创建不了
				// tex_img_CI.default_image_mem_prop_flag =
				// VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
			},
			Anni::RenderGraphV1::AttachUsage(
				DeferedRendering::C_depth_stencil_format,
				Anni::AttachmentInfo{
					.attachment_index = Vk::AttachmentIndex<1>,
					.attach_type = Anni::AttachmentType::DepthAttachment,
					.clear_value = vk::ClearDepthStencilValue{0.0f, 0u}
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
					.stage_mask = vk::PipelineStageFlagBits2::eLateFragmentTests,
					.layout_inpass = vk::ImageLayout::eDepthStencilAttachmentOptimal
				},
				Anni::RsrcAccessTypeRG::Write));

		auto& MSColorResolveTar = DeferedComPass.Out(
			"SwapImage",
			frame_datas[img_index].swapchain_attachment,
			Anni::RenderGraphV1::AttachUsage(
				DeferedRendering::C_color_attch_format,

				Anni::AttachmentInfo{
					.attachment_index = Vk::AttachmentIndex<0>,
					.attach_type = Anni::AttachmentType::ResolveOpTargetAttachment,
					.clear_value = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}
				},

				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eColorAttachmentWrite,
					.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
					.layout_inpass = vk::ImageLayout::eColorAttachmentOptimal
				},

				Anni::RsrcAccessTypeRG::Write));

		auto& MSDepthStencilResolveTar = DeferedComPass.Out(
			"DepthImage",
			VkTexture::Descriptor{
				.tex_img_CI = CI::GetDepthImgCI(swapchain_manager),
				.img_view_CI = CI::PopulateDepthImgViewCI(swapchain_manager),
				.sampler_CI = std::nullopt,
			},
			[](VkTexture::Descriptor& desco)
			{
			},
			Anni::RenderGraphV1::AttachUsage(
				DeferedRendering::C_depth_stencil_format,

				Anni::AttachmentInfo{
					.attachment_index = Vk::AttachmentIndex<1>,
					.attach_type = Anni::AttachmentType::ResolveOpTargetAttachment,
					.clear_value = vk::ClearDepthStencilValue{0.0f, 0u}
				},
				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite |
					vk::AccessFlagBits2::eDepthStencilAttachmentRead,

					.stage_mask = vk::PipelineStageFlagBits2::eEarlyFragmentTests,
					.layout_inpass = vk::ImageLayout::eDepthStencilAttachmentOptimal
				},

				Anni::RsrcAccessTypeRG::Write));

		MSColorAttach.GetRsrcAndUsage().PreBindResolveTarget(MSColorResolveTar.GetRsrcAndUsage());
		MSDepthStencilAttach.GetRsrcAndUsage().PreBindResolveTarget(MSDepthStencilResolveTar.GetRsrcAndUsage());

		auto& PresentPass = render_graph_v1.AddGfxPassNode<RenderGraphV1::PresentPass>("PresentPass");
		PresentPass.Out(
			MSColorResolveTar,
			Anni::RenderGraphV1::AttachUsage(
				DeferedRendering::C_color_attch_format,
				Anni::AttachmentInfo{},

				Anni::ImgSyncInfo{
					.access_mask = vk::AccessFlagBits2::eMemoryRead,
					.stage_mask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
					.layout_inpass = vk::ImageLayout::ePresentSrcKHR
				},
				Anni::RsrcAccessTypeRG::Write
			));

		// RenderGraph编译
		render_graph_v1.Compile();
		// RenderGraph执行
		render_graph_v1.CmdBufRecordingAndExecuting(img_index, cur_frame,
		                                            frame_num_semaphores[cur_frame % Vk::MAX_FRAMES_OVERLAP]->GetRaw());
	}

	void RealtimeRenderer::Render(float time_diff)
	{
		static uint64_t cur_frame = 0;

		uint32_t img_index = 0;
		const vk::AcquireNextImageInfoKHR acquire_next_img_info(
			swapchain_manager.GetSwapChain(),
			UINT64_MAX,
			present_finished_semaphores[cur_frame]->GetRaw(),
			VK_NULL_HANDLE);
		const vk::ResultValue<uint32_t> result = 
			device_manager.GetLogicalDevice().acquireNextImage2KHR(acquire_next_img_info);

		switch (result.result)
		{
		case vk::Result::eSuccess:
			img_index = result.value;
			break;
		case vk::Result::eTimeout:
			return;
			break;
		case vk::Result::eNotReady:
		case vk::Result::eSuboptimalKHR:
			// do something meaningful
			ASSERT_WITH_MSG(false, "Failed to acquire swapchain image!");
			break;
		default:
			// should not happen, as other return codes are considered to be an error
			// and throw an exception
			ASSERT_WITH_MSG(false, "Failed to acquire swapchain image!");
			break;
		}

		frame_datas[img_index].swapchain_attachment->present_finished_sem = present_finished_semaphores[cur_frame %
			Vk::MAX_FRAMES_OVERLAP];
		frame_datas[img_index].swapchain_attachment->ready_for_present_sem = ready_present_semaphores[cur_frame %
			Vk::MAX_FRAMES_OVERLAP];


		UpdateUniformBuffer(img_index);
		// if (nullptr != image_fences[img_index])  // 防止第一帧直接爆了
		//{
		//	vkWaitForFences(device_manager.GetLogicalDevice(), 1,
		//&image_fences[img_index]->GetRawFence(), VK_TRUE, UINT64_MAX);
		// }

		// image_fences[img_index] = frame_fences[cur_frame];

		// vkResetFences(device_manager.GetLogicalDevice(), 1,
		// &frame_fences[cur_frame]->GetRawFence());

		// UI Reset command buffers
		// imgui_UI.ResetCommandPoolAndBeginCommandBuffer(img_index);

		//// UI Describing UI
		// imgui_UI.DescribingUI();
		// imgui_UI.TestDescribingUI(camera->GetEyePos());

		//// UI command buffer recording
		// imgui_UI.RenderingCommandRecord(img_index);

		UpdateCamera(time_diff);
		//**********************************************************************************************************************
		RenderGraphRendering(img_index, cur_frame);

		cur_frame = (cur_frame + 1);

		//// Graphics Rendering command buffer submitting.
		// VkSubmitInfo gfx_submit_info{};
		// gfx_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// const VkSemaphore gfx_wait_semaphores[] = {
		// image_available_semaphores->GetOne(cur_frame) };

		// constexpr VkPipelineStageFlags gfx_wait_stages[] = {
		//	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		// gfx_submit_info.waitSemaphoreCount = 1;
		// gfx_submit_info.pWaitSemaphores = gfx_wait_semaphores;
		// gfx_submit_info.pWaitDstStageMask = gfx_wait_stages;

		// const auto graphics_command_buffer =
		//	command_manager.GetGraphicsCommandBuffers()[img_index];

		//// CommandBufferRecording(graphics_command_buffer, img_index);

		// const std::array<VkCommandBuffer, 1> submit_command_buffers = {
		//	graphics_command_buffer };

		// gfx_submit_info.commandBufferCount =
		//	static_cast<uint32_t>(submit_command_buffers.size());
		////&graphics_command_buffers[imageIndex] 用的frame buffer就是swap
		//// image[imageIndex]
		// gfx_submit_info.pCommandBuffers = submit_command_buffers.data();

		// const VkSemaphore gfx_signal_semaphores[] = {
		//	general_rendering_finished_semaphores->GetOne(
		//		cur_frame) };  //
		// graphics_command_buffers执行完以后会signal这里，随后ui_command_buffer就知道可以开始渲染UI了。
		//	gfx_submit_info.signalSemaphoreCount = 1;
		// gfx_submit_info.pSignalSemaphores = gfx_signal_semaphores;

		//// VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1,
		//// &gfx_submit_info, frame_fences->GetOne(cur_frame)))
		// VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1,
		//	&gfx_submit_info, VK_NULL_HANDLE))
		//	//**********************************************************************************************************************
		//	// UI rendering command buffer submitting.
		//	VkSubmitInfo ui_submit_info{};
		// const auto UI_command_buffer = imgui_UI.GetCommandBuffer(img_index);
		// ui_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// const VkSemaphore ui_wait_semaphores[] = {
		//	general_rendering_finished_semaphores->GetOne(cur_frame) };
		// constexpr VkPipelineStageFlags ui_wait_stages[] = {
		//	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		// ui_submit_info.waitSemaphoreCount = 1;
		// ui_submit_info.pWaitSemaphores = ui_wait_semaphores;
		// ui_submit_info.pWaitDstStageMask = ui_wait_stages;

		// const std::array<VkCommandBuffer, 1> ui_submit_command_buffers = {
		//	UI_command_buffer };

		// ui_submit_info.commandBufferCount =
		//	static_cast<uint32_t>(ui_submit_command_buffers.size());
		// ui_submit_info.pCommandBuffers = ui_submit_command_buffers.data();

		// const VkSemaphore ui_signal_semaphores[] = {
		//	UI_rendering_finished_semaphores->GetOne(
		//		cur_frame) };  //
		// ui_command_buffers执行完以后会signal这里，随后presentation
		//	// engine知道渲染完成可以展示了。
		//	ui_submit_info.signalSemaphoreCount = 1;
		// ui_submit_info.pSignalSemaphores = ui_signal_semaphores;

		//// vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &ui_submit_info,
		//// frame_fences->GetOne(cur_frame));
		// VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1,
		//	&ui_submit_info,
		//	frame_fences->GetOne(cur_frame)))
		//	//**********************************************************************************************************************

		//	VkPresentInfoKHR presentInfo{};
		// presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		// presentInfo.waitSemaphoreCount = 1;
		// presentInfo.pWaitSemaphores = ui_signal_semaphores;

		// const VkSwapchainKHR swapChains[] = { swapchain_manager.GetSwapChain()
		// }; 	presentInfo.pSwapchains = swapChains; 	presentInfo.swapchainCount =
		// 1; presentInfo.pImageIndices = &img_index; presentInfo.pResults = nullptr;
		// // Optional

		// result = vkQueuePresentKHR(device_manager.GetPresentQueue(),
		//	&presentInfo);

		//// if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
		//	// framebufferResized) { 	framebufferResized = false;
		//	//	//recreateSwapChain();
		////}

		// if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		//{
		//	// TODO: recreation of swapchain
		//	// recreateSwapChain();
		//	return;
		// }
		// if (result != VK_SUCCESS)
		//{
		//	throw std::runtime_error("failed to present swap chain image!");
		// }
		// cur_frame = (cur_frame + 1) % Vk::MAX_FRAMES_OVERLAP;
	}
} // namespace Anni::Renderer
