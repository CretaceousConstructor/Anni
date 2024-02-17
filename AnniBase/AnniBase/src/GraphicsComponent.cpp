#include "GraphicsComponent.h"
namespace Anni
{
	GraphicsComponent::GraphicsComponent(InstanceWrapper& instance_, DeviceManager& device_manager_,
		QueueManager& queue_manager_,  SwapchainManager& swapchain_manager_, WindowsSys& window_) :
		instance(instance_),
		device_manager(device_manager_),
		window(window_),
		queue_manager(queue_manager_),
		swapchain_manager(swapchain_manager_)
	{

	}

	DeviceManager& GraphicsComponent::DeviceMan()
	{
		return device_manager;
	}

	SwapchainManager& GraphicsComponent::SwapchainMan()
	{
		return swapchain_manager;
	}


	WindowsSys& GraphicsComponent::Window()
	{
		return window;
	}

	QueueManager& GraphicsComponent::QueueMan()
	{
		return queue_manager;
	}

	InstanceWrapper& GraphicsComponent::Instance()
	{
		return instance;
	}

}        // namespace Anni
