#pragma once
#include "AnniBase/IApp.h"
#include "AnniBase/GraphicsComponent.h"
#include "RealtimeRenderer.h"
#include <memory>



namespace Anni
{
	class GraphicsApp final : public IApp
	{
	public:
		void Run();
		explicit GraphicsApp();
		~GraphicsApp() override = default;
	private:
		vk::raii::Context   context;
		InstanceWrapper     instance;
		WindowsSys          window;
		DeviceManager       device_manager;
		QueueManager        queue_manager;
		SwapchainManager    swap_chain_manager;

		GraphicsComponent   gfx;
	private:
		std::unique_ptr<Renderer::IRenderer> renderer;
	private:
		void CreateRenderer();
		void MainLoop();
	};

}        // namespace Anni
