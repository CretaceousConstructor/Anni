#pragma once

//#include "Vk.h"
//#include "Material.h"
//#include "ModelStrcture.h"
//
//namespace Anni
//{
//	class NonPbrMat : public Material
//	{
//	public:
//		NonPbrMat(GraphicsComponent& gfx_);
//		~NonPbrMat() override = default;
//
//		NonPbrMat() = delete;
//		NonPbrMat(const NonPbrMat&) = delete;
//		NonPbrMat& operator=(const NonPbrMat&) = delete;
//		NonPbrMat(NonPbrMat&&) = delete;
//		NonPbrMat& operator=(NonPbrMat&&) = delete;
//
//		glm::vec4 baseColorFactor{ glm::vec4(1.0f) };
//		uint32_t  baseColorTextureIndex{};
//
//		uint32_t normalTextureIndex{};
//		uint32_t emissiveTextureIndex{};
//		uint32_t occlusionTextureIndex{};
//
//	public:
//		enum class AlphaMode
//		{
//			ALPHAMODE_OPAQUE,
//			ALPHAMODE_MASK,
//			ALPHAMODE_BLEND
//		};
//
//		AlphaMode alphaMode{ AlphaMode::ALPHAMODE_OPAQUE };
//		float     alphaCutoff{ 0. };
//		bool      doubleSided{ false };
//
//		//***********************************************************************
//		[[nodiscard]] constexpr uint32_t GetRequiredDescirpotrCount() const override;
//
//		VkDescriptorSet AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture>& textures, const std::vector<std::shared_ptr<VkTexture>>& images, Vk::ModelLoadingOption option) override;
//		void            ModifyPipelineCI(GFXPipelineCI& pipeline_CI) override;
//
//		static VkDescriptorSetLayout GetDescriptorSetLayout();
//		static VkDescriptorSetLayout GetDescriptorSetLayoutBindlessRendering();
//
//	public:
//		static VkPipelineLayout CreatePipelineLayout(const DeviceManager& device_manager, const std::vector<VkDescriptorSetLayout>& set_layouts, const std::vector<VkPushConstantRange>& push_constant_ranges);
//
//		static void Register(GraphicsComponent& gfx);
//		static void UnRegister(GraphicsComponent& gfx);
//
//	private:
//		inline static VkDescriptorSetLayout desc_layout{ nullptr };
//		inline static VkDescriptorSetLayout desc_layout_bindless_rendering{ nullptr };
//
//	private:
//		struct MaterialSpecializationData
//		{
//			bool  alphaMask;
//			float alphaMaskCutoff;
//		};
//
//		MaterialSpecializationData            material_specialization_data{};
//		std::vector<VkSpecializationMapEntry> specialization_map_entries;
//	};
//
//}        // namespace Anni
