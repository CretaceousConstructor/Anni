#pragma once
#include "AnniUtil.h"
#include "AnniVkHeader.h"
#include "DeviceManager.h"

namespace Anni
{
	namespace CI
	{
		static VkDescriptorSetLayoutCreateInfo
			GetDescriptorSetLayoutCI(
				const std::vector<VkDescriptorSetLayoutBinding> bindings,
				VkDescriptorSetLayoutCreateFlags flags = 0,
				const void* pNext = VK_NULL_HANDLE);
	}

	namespace DescVk
	{

		VkDescriptorSetLayoutBinding
			GetDescriptorSetLayoutBinding(uint32_t binding,
				VkDescriptorType type,
				VkShaderStageFlags stage_flags,
				uint32_t descriptor_count = 1,
				const VkSampler* pImmutableSamplers = nullptr);

	}

	class DescriptorSetLayoutWrapper
	{
	public:
		friend class DescriptorLayoutManager;
		~DescriptorSetLayoutWrapper()
		{
			vkDestroyDescriptorSetLayout(
				device_manager.GetLogicalDevice(), set_layout, VK_NULL_HANDLE);
		}

		VkDescriptorSetLayout GetRawDescriptorSetLayout()
		{
			return set_layout;
		}

	public:
		DescriptorSetLayoutWrapper() = delete;
		DescriptorSetLayoutWrapper(const DescriptorSetLayoutWrapper&) = delete;
		DescriptorSetLayoutWrapper(DescriptorSetLayoutWrapper&&) = delete;
		DescriptorSetLayoutWrapper& operator=(const DescriptorSetLayoutWrapper&) =
			delete;
		DescriptorSetLayoutWrapper& operator=(DescriptorSetLayoutWrapper&&) = delete;

	private:
		DescriptorSetLayoutWrapper(DeviceManager device_manager_,
			VkDescriptorSetLayout set_layout_)
			: device_manager(device_manager_)
			, set_layout(set_layout_)
		{
		}

		DeviceManager& device_manager;
		VkDescriptorSetLayout set_layout;
	};

	class DescriptorLayoutManager
	{

		template<typename Mat>
		bool IsMatDescriptorSetLayoutCreated();

	public:
		template<typename Mat>
		std::shared_ptr<DescriptorSetLayoutWrapper> GetMatDescriptorSetLayout(
			const std::vector<VkDescriptorSetLayoutBinding> bindings,
			VkDescriptorSetLayoutCreateFlags flags = 0,
			const void* pNext = VK_NULL_HANDLE);

		std::unique_ptr<DescriptorSetLayoutWrapper> ProduceDescriptorSetLayoutUnsafe(
			const VkDescriptorSetLayoutCreateInfo desc_set_layout_CI)
		{

			VkDescriptorSetLayout descriptor_set_layout;
			VK_CHECK_RESULT(
				vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(),
					&desc_set_layout_CI,
					VK_NULL_HANDLE,
					&descriptor_set_layout));
			return std::make_unique<DescriptorSetLayoutWrapper>(device_manager,
				descriptor_set_layout);
		}

	public:
		DescriptorLayoutManager(DeviceManager& device_manager_);
		~DescriptorLayoutManager();

	private:
		DeviceManager& device_manager;

	private:
		std::unordered_map<void (*)(),
			std::shared_ptr<DescriptorSetLayoutWrapper>,
			Util::TypeHash::TypeIdHash,
			Util::TypeHash::TypeIdEqual>
			mat_deslayout_map;
	};

	template<typename Mat>
	inline bool
		DescriptorLayoutManager::IsMatDescriptorSetLayoutCreated()
	{
		if (mat_deslayout_map.contains(&Util::TypeHash::TypeIdentifier<Mat>::id))
		{
			return true;
		}
		return false;
	}

	template<typename Mat>
	inline std::shared_ptr<DescriptorSetLayoutWrapper>
		DescriptorLayoutManager::GetMatDescriptorSetLayout(
			const std::vector<VkDescriptorSetLayoutBinding> bindings,
			VkDescriptorSetLayoutCreateFlags flags,
			const void* pNext)
	{
		if (mat_deslayout_map.contains(&Util::TypeHash::TypeIdentifier<Mat>::id))
		{
			return mat_deslayout_map[&Util::TypeHash::TypeIdentifier<Mat>::id];
		}

		auto set_layout_CI = CI::GetDescriptorSetLayoutCI(bindings, flags, pNext);
		VkDescriptorSetLayout descriptor_set_layout;
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(),
			&set_layout_CI,
			nullptr,
			&descriptor_set_layout))
			mat_deslayout_map[&Util::TypeHash::TypeIdentifier<Mat>::id] =
			descriptor_set_layout;

		return std::make_shared<DescriptorSetLayoutWrapper>(device_manager.GetLogicalDevice(),descriptor_set_layout);
	}

}