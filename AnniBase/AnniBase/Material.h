#pragma once
#include "GraphicsComponent.h"
#include "ModelStrcture.h"
#include "VkDescriptorManager.h"
#include "VkManagerBundle.h"
#include "VkMetaInfo.h"

namespace Anni
{

	// Define a concept for a class with two specific static functions
	template <typename T>
	concept HasRegUnRegFunctions = requires(GraphicsComponent & gfx)
	{
		{
			T::Register(gfx)
		} -> std::same_as<void>; // Adjust the return type if necessary
		{
			T::UnRegister(gfx)
		} -> std::same_as<void>; // Adjust the return type if necessary
	};


	class Material
	{
	public:
		explicit Material(GraphicsComponent& gfx_);

		virtual VkDescriptorSet AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture>& textures, const std::vector<std::shared_ptr<VkTexture>>& images, Vk::ModelLoadingOption option) = 0;

		virtual void ModifyPipelineCI(GFXPipelineCI& pipeline_CI) = 0;

		[[nodiscard]] virtual uint32_t GetRequiredDescirpotrCount() const = 0;

		template <HasRegUnRegFunctions M>
		static void Register(GraphicsComponent& gfx);

		template <HasRegUnRegFunctions M>
		static void UnRegister(GraphicsComponent& gfx);


	protected:
		GraphicsComponent& gfx;
		DeviceManager& device_manager;

		virtual ~Material() = default;

		Material() = delete;
		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
		Material(Material&&) = delete;
		Material& operator=(Material&&) = delete;

	};

	template <HasRegUnRegFunctions M>
	inline void Material::Register(GraphicsComponent& gfx)
	{
		M::Register(gfx);
	}

	template <HasRegUnRegFunctions M>
	inline void Material::UnRegister(GraphicsComponent& gfx)
	{
		M::UnRegister(gfx);
	}

}        // namespace Anni
