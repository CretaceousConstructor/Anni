#pragma once
#include "GraphicsComponent.h"
#include "AnniVkHeader.h"
#include <unordered_map>
namespace Anni
{

	namespace DescriptorManagement
	{
		template <typename T>
		concept HasDescType =
			requires(T t)
		{
			{
				t.desc_type
			} -> std::same_as<VkDescriptorType&>;
		};


		template <HasDescType T, typename... Args>
		void FillDescriptorPoolSizeMap(std::unordered_map<VkDescriptorType, uint32_t, Vk::EnumClassHash>& result_map, T&& first, Args &&...args)
		{
			if (!result_map.contains(first.desc_type))
			{
				result_map.emplace(first.desc_type, 1u);
			}
			else
			{
				++result_map[first.desc_type];
			}
			if constexpr (0 == sizeof...(args))
			{
				return;
			}
			else
			{
				return FillDescriptorPoolSizeMap(result_map, std::forward<Args>(args)...);
			}
		}
		template <HasDescType... Args>
		void FillDescriptorPoolSizeMap(std::unordered_map<VkDescriptorType, uint32_t, Vk::EnumClassHash>& result_map, Args &&...args)
		{
			if (0 == sizeof...(args))
			{
				return;
			}
		}
	}        // namespace DescriptorManagement
	class VkDescriptorManagerV0
	{
	public:
		VkDescriptorManagerV0(GraphicsComponent& gfx);
		~VkDescriptorManagerV0();
		VkDescriptorManagerV0() = delete;
		VkDescriptorManagerV0(const VkDescriptorManagerV0&) = delete;
		VkDescriptorManagerV0& operator=(const VkDescriptorManagerV0&) = delete;
		VkDescriptorManagerV0(VkDescriptorManagerV0&&) = delete;
		VkDescriptorManagerV0& operator=(VkDescriptorManagerV0&&) = delete;

	public:
		VkDescriptorPool      ProduceDescriptorPoolUnsafe(const VkDescriptorPoolCreateInfo& desc_pool_CI, const VkAllocationCallbacks* pAllocator = nullptr);
		void                  DestroyDescriptorPoolUnsafe(VkDescriptorPool desc_pool);
		VkDescriptorSetLayout ProduceDescriptorSetLayoutUnsafe(const VkDescriptorSetLayoutCreateInfo& desc_set_layout_CI, const VkAllocationCallbacks* pAllocator = nullptr);
		void                  DestroyDescriptorSetLayoutUnsafe(VkDescriptorSetLayout desc_set_layout);
		//use a map to handle  VkAllocationCallbacks shit,use VkAllocationCallbacks as key of the map;
	private:
		/* * *"Each object of a dispatchable type must have a unique handle value during its lifetime." Non-dispatchable handle types are a 64-bit integer type whose meaning is implementation- dependent. If the privateData feature is enabled for a VkDevice, each object of a non-dispatchable type created on that device must have a handle value that is unique among objects created on that device, for the duration of the object’s lifetime. Otherwise, non-dispatchable handles may encode object information directly in the handle rather than acting as a reference to an underlying object, and thus may not have unique handle values. If handle values are not unique, then destroying one such handle must not cause identical handles of other types to become invalid, and must not cause identical handles of the same type to become invalid if that handle value has been created more times than it has been destroyed. * */
		//std::mutex desc_layout_mutex;
		//std::mutex desc_pool_mutex;
		std::unordered_map<VkDescriptorSetLayout, uint32_t> desc_set_layouts;
		std::unordered_map<VkDescriptorPool, uint32_t>      desc_set_pools;

	private:
		std::unordered_map<VkDescriptorSetLayout, uint32_t> desc_set_layouts_unsafe;
		std::unordered_map<VkDescriptorPool, uint32_t>      desc_set_pools_unsafe;

	private:
		DeviceManager& device_manager;
	};

}
