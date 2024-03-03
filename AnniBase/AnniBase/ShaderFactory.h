#pragma once
#include "AnniUtil.h"
#include "GraphicsComponent.h"
#include "ShaderWrapper.h"
#include <Windows.h>
#include <codecvt>
#include <dxcapi.h>
#include <locale>
#include <string>
#include <vector>
#include <wrl/client.h>

namespace Anni
{
	class ShaderFactory
	{
	public:
		ShaderFactory(GraphicsComponent& gfx_);

		[[nodiscard]] std::shared_ptr<ShaderWrapper> GetShader(const std::string& path, VkShaderStageFlagBits stage) const;
		//[[nodiscard]] std::shared_ptr<VkShaderExtWrapper> GetShaderExt(const std::string &path, VkShaderCreateInfoEXT shader_CI) const;

	private:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;

	private:
		//MS bullshit
		Microsoft::WRL::ComPtr<IDxcUtils>     dxc_utils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxc_compiler;
	};

}        // namespace Anni
