#include "GraphicsApp.h"

namespace Anni
{
	GraphicsApp::GraphicsApp()
		:
		context(),
		instance(context),
		window(instance),
		device_manager(instance, window),
		queue_manager(device_manager, window),
		swap_chain_manager(device_manager, window),

		gfx(instance, device_manager, queue_manager, swap_chain_manager, window)
	{
	}

	void GraphicsApp::Run()
	{
		CreateRenderer();
		renderer->RenderingPreparation();        //多态调用，这个函数会给智能指针一个具体的实例对象
		MainLoop();
	}

	void GraphicsApp::MainLoop()
	{
		float time_diff = 0.f;

		while (!glfwWindowShouldClose(const_cast<GLFWwindow*>(window.GetWindowPtr())))
		{
			glfwPollEvents();
			const auto tStart = glfwGetTime();

			renderer->Render(time_diff);

			const auto tEnd = glfwGetTime();
			time_diff = static_cast<float>(tEnd - tStart);
		}

		device_manager.GetLogicalDevice().waitIdle();
	}




	void GraphicsApp::CreateRenderer()
	{
		renderer = std::make_unique<Renderer::RealtimeRenderer>(gfx);
	}


}        // namespace Anni
