#pragma once

#include "GraphicsComponent.h"
#include "VkSubpassWrapper.h"
#include <vector>

//class VkRenderpassWrapper
//{
//  public:
//	VkRenderpassWrapper(std::string _renderpass_name, std::vector<VkAttachmentInfo> _attachment_infos, std::vector<VkSubpassDependency> _dependencies, 	std::vector<std::shared_ptr<VkSubpassWrapper>>  _subpasses, GraphicsComponent& _gfx);
//
//	~VkRenderpassWrapper();
//
//
//	VkRenderpassWrapper() = delete;
//	VkRenderpassWrapper(const VkRenderpassWrapper &) = delete;
//	VkRenderpassWrapper &operator=(const VkRenderpassWrapper &) = delete;
//
//	VkRenderpassWrapper(VkRenderpassWrapper &&) = delete;
//	VkRenderpassWrapper &operator=(VkRenderpassWrapper &&) = delete;
//
//
//
//	[[nodiscard]] const std::vector<VkFramebuffer>& GetFrameBuffers() const;
//	[[nodiscard]] VkRenderPass GetRenderpass() const;
//
//	const std::vector<VkAttachmentInfo> attachment_infos;
//	const std::vector<VkSubpassDependency>     dependencies;
//	const std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses;
//	const std::string  render_pass_name;
//
//
//private:
//	GraphicsComponent &gfx;
//	const DeviceManager & device_manager;
//	const SwapchainManager &swapchain_manager;
//
//  private:
//	VkRenderPass render_pass{};
//	std::vector<VkFramebuffer> frame_buffers;
//};
