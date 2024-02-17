#pragma once

#include "DeviceManager.h"
#include <unordered_set>
//command buffer���͵ı����� dispatchable�ģ����Կ��Ե���std map��keyֵ��
//
//namespace Anni
//{
//	class VkCommandManager   //ʹ��rendergraph�Ժ��������Ҫ���
//	{
//	public:
//		VkCommandManager(DeviceManager& _device_manager, size_t num_of_graphics_command_buffers, size_t num_of_transfer_command_buffer);
//		~VkCommandManager() = default;
//		VkCommandManager() = delete;
//
//		VkCommandManager(const VkCommandManager&) = delete;
//		VkCommandManager& operator=(const VkCommandManager&) = delete;
//
//		VkCommandManager(VkCommandManager&&) = delete;
//		VkCommandManager& operator=(VkCommandManager&&) = delete;
//
//		//TODO:�����commandbuffer�Ĺ����ǲ���ȫ�ģ���취
//		[[nodiscard]] static VkCommandBuffer BeginSingleTimeCommands(const VkCommandPool& command_pool, const VkDevice& device);
//		static void                          EndSingleTimeCommands(const VkCommandPool& command_pool, const VkDevice& device, const VkCommandBuffer command_buffer, const VkQueue& command_quque);
//
//		static void CreateCommandBuffer(const VkDevice& device, const VkCommandPool& commandpool, VkCommandBuffer& CommandBuffer, VkCommandBufferLevel level);
//
//		static void BeginCommandBuffers(const std::vector<VkCommandBuffer>& command_buffers);
//		static void EndCommandBuffers(const std::vector<VkCommandBuffer>& command_buffers);
//
//		static void BeginCommandBuffers(VkCommandBuffer command_buffer);
//		static void EndCommandBuffers(VkCommandBuffer command_buffer);
//
//	private:
//		DeviceManager& device_manager;
//
//	public:
//		//COMMAND POOL
//		const VkCommandPool graphics_command_pool{ nullptr };
//		const VkCommandPool transfer_command_pool{ nullptr };
//
//		[[nodiscard]] const std::vector<VkCommandBuffer>& GetGraphicsCommandBuffers() const;
//		[[nodiscard]] const std::vector<VkCommandBuffer>& GetTransferCommandBuffers() const;
//
//	private:
//		//COMMAND BUFFERS
//		std::vector<VkCommandBuffer> graphics_command_buffers;        //3
//		std::vector<VkCommandBuffer> transfer_command_buffer;
//
//		//COMMAND BUFFERS
//		std::unordered_set<VkCommandBuffer> command_buffers_pool;
//	};
//}        // namespace Anni