#pragma once
#include "AnniVkHeader.h"
#include "QueueManager.h"



namespace Anni
{

	struct WaitValue
	{
		auto operator<=>(const WaitValue& other) const = default;
		uint64_t val;
	};

	struct SignalValue
	{

		auto operator<=>(const SignalValue& other) const = default;
		uint64_t val;
	};

	struct SemInsertInfoSafe
	{
		SemInsertInfoSafe() = delete;
		SemInsertInfoSafe(WaitValue wait_val_, vk::PipelineStageFlags2 wait_stages_);
		SemInsertInfoSafe(SignalValue signal_val_, vk::PipelineStageFlags2 signal_stages_);
		SemInsertInfoSafe(SignalValue signal_val_, vk::PipelineStageFlags2 signal_stages_, WaitValue wait_val_, vk::PipelineStageFlags2 wait_stages_);

	public:
		std::optional<WaitValue> wait_val;
		std::optional<SignalValue> signal_val;
		vk::PipelineStageFlags2 wait_stages;
		vk::PipelineStageFlags2 signal_stages;
	};

	struct BufSyncInfo
	{
		vk::AccessFlags2        access_mask;
		vk::PipelineStageFlags2 stage_mask;

		std::optional<Vk::BufferSubRange> buf_subrange;
	};


	struct ImgSyncInfo
	{
		vk::AccessFlags2        access_mask;
		vk::PipelineStageFlags2 stage_mask;
		vk::ImageLayout         layout_inpass;

		std::optional<vk::ImageSubresourceRange> img_subrange;
	};

	struct Buf2BufCopyInfo
	{
		friend class BufferFactory;
		friend class Buffer;
		enum class TransferType
		{
			Unknown,
			H2D,
			D2H,
			D2D_DST,
			D2D_SRC
		};

		VkDeviceSize size;
		VkDeviceSize srcOffset;
		VkDeviceSize dstOffset;
		TransferType trans_type;
	private:
		Queue* queue;
		std::shared_ptr<TimelineSemWrapper> sem;
	};


}        // namespace Anni


