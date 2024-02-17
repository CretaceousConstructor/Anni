#pragma once
//
//#include "Material.h"
//#include "Vk.h"
//namespace Anni
//{
//
//
//	class PbrMetallicMat : public Material
//	{
//	public:
//		PbrMetallicMat(GraphicsComponent& gfx_);
//		~PbrMetallicMat() override = default;
//
//		PbrMetallicMat() = delete;
//		PbrMetallicMat(const PbrMetallicMat&) = delete;
//		PbrMetallicMat& operator=(const PbrMetallicMat&) = delete;
//		PbrMetallicMat(PbrMetallicMat&&) = delete;
//		PbrMetallicMat& operator=(PbrMetallicMat&&) = delete;
//
//		glm::vec4 baseColorFactor{ glm::vec4(1.0f) };
//		uint32_t  baseColorTextureIndex{};
//
//		float    metallicFactor{ 1.0f };
//		float    roughnessFactor{ 1.0f };
//		uint32_t metallicRoughnessTextureIndex{};
//
//		uint32_t metallicTextureIndex{};
//		uint32_t roughnessTextureIndex{};
//
//		uint32_t normalTextureIndex{};
//
//		uint32_t emissiveTextureIndex{};
//
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
//		float     alphaCutoff{ 0.f };
//		bool      doubleSided{ false };
//
//		//***********************************************************************
//		[[nodiscard]] constexpr uint32_t GetRequiredDescirpotrCount() const override;
//		VkDescriptorSet                  AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture>& textures, const std::vector<std::shared_ptr<VkTexture>>& images, Vk::ModelLoadingOption option) override;
//		void                             ModifyPipelineCI(GFXPipelineCI& pipeline_CI) override;
//
//	public:
//		static VkDescriptorSetLayout GetDescriptorSetLayout();
//		static VkDescriptorSetLayout GetDescriptorSetLayoutBindlessRendering();
//		//static VkPipelineLayout      GetPipelineLayout();
//
//		//static VkDescriptorSetLayout CreateDesciptorSetLayout(const DeviceManager &device_manager);
//		//static VkPipelineLayout      CreatePipelineLayout(const DeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges);
//
//		//static void CleanUpMaterial(const DeviceManager &device_manager);
//
//		static void Register(GraphicsComponent& gfx);
//		static void UnRegister(GraphicsComponent& gfx);
//
//	private:
//		inline static VkDescriptorSetLayout desc_layout{ nullptr };
//		inline static VkDescriptorSetLayout desc_layout_bindless_rendering{ nullptr };
//		//inline static bool                  desc_layout_established{false};
//
//		//inline static VkPipelineLayout pipe_layout{nullptr};
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
//}        // namespace Anni
