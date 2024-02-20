#pragma once
#include "AnniUtil.h"
#include "AnniVkHeader.h"
#include "DeviceManager.h"

namespace Anni
{
	namespace CI
	{
		static vk::DescriptorSetLayoutCreateInfo
			GetDescriptorSetLayoutCI(
				const std::vector<vk::DescriptorSetLayoutBinding> bindings,
				vk::DescriptorSetLayoutCreateFlags flags = vk::DescriptorSetLayoutCreateFlags(VK_ZERO_FLAG),
				const void* pNext = VK_NULL_HANDLE);
	}



	//class DescriptorSetLayoutWrapper
	//{
	//public:
	//	friend class DescriptorLayoutManager;
	//	~DescriptorSetLayoutWrapper()
	//	{
	//		vkDestroyDescriptorSetLayout(
	//			device_manager.GetLogicalDevice(), set_layout, VK_NULL_HANDLE);
	//	}

	//	VkDescriptorSetLayout GetRaw()
	//	{
	//		return set_layout;
	//	}

	//public:
	//	DescriptorSetLayoutWrapper() = delete;
	//	DescriptorSetLayoutWrapper(const DescriptorSetLayoutWrapper&) = delete;
	//	DescriptorSetLayoutWrapper(DescriptorSetLayoutWrapper&&) = delete;
	//	DescriptorSetLayoutWrapper& operator=(const DescriptorSetLayoutWrapper&) =
	//		delete;
	//	DescriptorSetLayoutWrapper& operator=(DescriptorSetLayoutWrapper&&) = delete;

	//private:
	//	DescriptorSetLayoutWrapper(DeviceManager device_manager_,
	//		VkDescriptorSetLayout set_layout_)
	//		: device_manager(device_manager_)
	//		, set_layout(set_layout_)
	//	{
	//	}

	//	DeviceManager& device_manager;
	//	VkDescriptorSetLayout set_layout;
	//};



	class DescriptorLayoutManager
	{

		template<typename Mat>
		bool IsMatDescriptorSetLayoutCreated();

	public:
		template<typename Mat>
		vk::DescriptorSetLayout GetMatDescriptorSetLayout(
			vk::DescriptorSetLayoutCreateInfo set_layout_CI
		);

		//std::unique_ptr<DescriptorSetLayoutWrapper> ProduceDescriptorSetLayoutUnsafe(
		//	const VkDescriptorSetLayoutCreateInfo desc_set_layout_CI)
		//{

		//	VkDescriptorSetLayout descriptor_set_layout;
		//	VK_CHECK_RESULT(
		//		vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(),
		//			&desc_set_layout_CI,
		//			VK_NULL_HANDLE,
		//			&descriptor_set_layout));
		//	return std::make_unique<DescriptorSetLayoutWrapper>(device_manager, descriptor_set_layout);
		//}

	public:
		DescriptorLayoutManager(DeviceManager& device_manager_);
		~DescriptorLayoutManager() = default;

	private:
		DeviceManager& device_manager;

	private:
		std::unordered_map<void (*)(),
			vk::UniqueDescriptorSetLayout,
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
	inline vk::DescriptorSetLayout
		DescriptorLayoutManager::GetMatDescriptorSetLayout(
			vk::DescriptorSetLayoutCreateInfo set_layout_CI
		)
	{
		if (mat_deslayout_map.contains(&Util::TypeHash::TypeIdentifier<Mat>::id))
		{
			return mat_deslayout_map[&Util::TypeHash::TypeIdentifier<Mat>::id].get();
		}

		vk::UniqueDescriptorSetLayout descriptor_set_layout = device_manager.GetLogicalDevice().createDescriptorSetLayoutUnique(set_layout_CI);
		vk::DescriptorSetLayout result = descriptor_set_layout.get();
		mat_deslayout_map[&Util::TypeHash::TypeIdentifier<Mat>::id] = std::move(descriptor_set_layout);

		return result;

	}






}