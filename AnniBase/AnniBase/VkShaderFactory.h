#pragma once
#define NOMINMAX
#include "AnniUtil.h"
#include "GraphicsComponent.h"
#include "VkShaderWrapper.h"
#include <Windows.h>
#include <codecvt>
#include <dxc/dxcapi.h>
#include <locale>
#include <string>
#include <vector>
#include <wrl/client.h>

namespace Anni
{
class VkShaderFactory
{
  public:
	VkShaderFactory(GraphicsComponent &gfx_);

	[[nodiscard]] std::shared_ptr<VkShaderWrapper>    GetShader(const std::string &path, VkShaderStageFlagBits stage) const;
	[[nodiscard]] std::shared_ptr<VkShaderExtWrapper> GetShaderExt(const std::string &path, VkShaderCreateInfoEXT shader_CI) const;

  private:
	GraphicsComponent &gfx;
	DeviceManager &    device_manager;

  private:
	//MS bull shit
	Microsoft::WRL::ComPtr<IDxcUtils>     dxc_utils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxc_compiler;
};

}        // namespace Anni
