#pragma once
#include "GraphicsComponent.h"

namespace Anni
{
	class VkShaderWrapper
	{
	public:
		using Ptr = std::shared_ptr<VkShaderWrapper>;

	public:
		class ShaderCI
		{
		public:
			ShaderCI(VkShaderStageFlagBits stage_, std::string shader_path) :
				binding_stage(stage_),
				shader_path(std::move(shader_path))
			{
			}
			VkShaderStageFlagBits binding_stage{};
			std::string           shader_path{};
			std::string           entry_point{ "main" };
		};

		VkShaderWrapper(const ShaderCI& para_pack, GraphicsComponent& gfx_);

		VkShaderWrapper(VkShaderStageFlagBits binding_stage, std::string shader_path_, GraphicsComponent& gfx_);
		VkShaderWrapper(VkShaderStageFlagBits binding_stage_, const std::string& shader_path_, VkShaderModule shader_module_, GraphicsComponent& gfx_);

		~VkShaderWrapper();

		VkShaderWrapper() = delete;

		VkShaderWrapper(const VkShaderWrapper&) = delete;
		VkShaderWrapper& operator=(const VkShaderWrapper&) = delete;

		VkShaderWrapper(VkShaderWrapper&&) = delete;
		VkShaderWrapper& operator=(VkShaderWrapper&&) = delete;

		[[nodiscard]] vk::PipelineShaderStageCreateInfo GetShaderStageCI() const;

	private:
		void CreateShaderModule(const std::vector<char>& code);

	private:
		static std::vector<char> ReadFile(const std::string& filename);

	private:
		DeviceManager& device_manager;
	private:
		VkShaderModule        shader_module{};
		VkShaderStageFlagBits binding_stage{};
		std::string           entry_point{ "main" };
		std::string           shader_path{};
	};

	//class VkShaderExtWrapper
	//{
	//public:
	//	using Ptr = std::shared_ptr<VkShaderExtWrapper>;

	//public:
	//	VkShaderExtWrapper(VkShaderCreateInfoEXT shader_CI, VkShaderEXT shader, std::string shader_path);

	//	VkShaderExtWrapper(VkShaderStageFlagBits binding_stage, std::string shader_path_, GraphicsComponent& gfx_);

	//	VkShaderExtWrapper(VkShaderStageFlagBits binding_stage_, const std::string& shader_path_, VkShaderModule shader_module_, GraphicsComponent& gfx_);

	//	~VkShaderExtWrapper();

	//	VkShaderExtWrapper() = delete;

	//	VkShaderExtWrapper(const VkShaderExtWrapper&) = delete;
	//	VkShaderExtWrapper& operator=(const VkShaderExtWrapper&) = delete;

	//	VkShaderExtWrapper(VkShaderExtWrapper&&) = delete;
	//	VkShaderExtWrapper& operator=(VkShaderExtWrapper&&) = delete;

	//	[[nodiscard]] VkPipelineShaderStageCreateInfo GetShaderStageCI() const;

	//private:
	//	[[nodiscard]] void CreateShaderModule(const std::vector<char>& code);

	//private:
	//	DeviceManager& device_manager;
	//	VkShaderEXT           shader;
	//	VkShaderCreateInfoEXT shader_CI;
	//	std::string           shader_path{};
	//};

}        // namespace Anni
