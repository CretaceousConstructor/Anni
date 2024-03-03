#pragma once
#include "VirtualBuffer.h"
#include "VirtualTexture.h"


namespace Anni::RenderGraphV1
{
	template <typename T>
	concept RsrcT = std::is_same_v<T, VirtualBuffer> || std::is_same_v<T, VirtualTexture>;

	template <RsrcT R>
	struct SyncInfoType
	{
		//using type = void; // Default case
	};

	template <>
	struct SyncInfoType<VirtualBuffer>
	{
		using type = BufSyncInfo;
	};

	template <>
	struct SyncInfoType<VirtualTexture>
	{
		using type = ImgSyncInfo;
	};


	template <RsrcT R>
	struct VRsrcHandle
	{
		//using type = void; // Default case
	};

	template <>
	struct VRsrcHandle<VirtualBuffer>
	{
		using type = VirtualBuffer::Handle;
	};

	template <>
	struct VRsrcHandle<VirtualTexture>
	{
		using type = VirtualTexture::Handle;
	};


	template <typename R, typename S = typename SyncInfoType<R>::type>
	class SyncInfoSameQueue
	{
	public:
		SyncInfoSameQueue(
			S source_sync_info_,
			S target_sync_info_,
			typename VRsrcHandle<R>::type underlying_vrsrc_,
			IRsrcUsage::RsrcOrigin rsrc_origin_ = IRsrcUsage::RsrcOrigin::Unknown
		);


		void AddSemToWaitOn(std::shared_ptr<TimelineSemWrapper> sync_sem, uint64_t wait_val = 1)
		{
			wait_sync_semas.push_back(std::make_pair(std::move(sync_sem), wait_val));
		}

		void AddSemToSignal(std::shared_ptr<TimelineSemWrapper> sync_sem, uint64_t signal_val = 1)
		{
			signal_sync_semas.push_back(std::make_pair(std::move(sync_sem), signal_val));
		}


		void AddSemToWaitOn(std::shared_ptr<BinarySemWrapper> sync_sem)
		{
			wait_sync_semas_binary.push_back(std::move(sync_sem));
		}

		void AddSemToSignal(std::shared_ptr<BinarySemWrapper> sync_sem)
		{
			signal_sync_semas_binary.push_back(std::move(sync_sem));
		}

	public:
		S source_sync_info;
		S target_sync_info;
		typename VRsrcHandle<R>::type underlying_vrsrc;
		IRsrcUsage::RsrcOrigin rsrc_origin;

	public:
		std::vector<std::pair<std::shared_ptr<TimelineSemWrapper>, uint64_t>> signal_sync_semas;
		std::vector<std::pair<std::shared_ptr<TimelineSemWrapper>, uint64_t>> wait_sync_semas;

		std::vector<std::shared_ptr<BinarySemWrapper>> wait_sync_semas_binary;
		std::vector<std::shared_ptr<BinarySemWrapper>> signal_sync_semas_binary;
	};


	template <typename R, typename S = typename SyncInfoType<R>::type>
	class SyncInfoDiffQueue
	{
	public:
		SyncInfoDiffQueue(
			S source_sync_info_,
			S target_sync_info_,
			Queue* source_queue_,
			Queue* target_queue_,
			typename VRsrcHandle<R>::type underlying_vrsrc_,
			IRsrcUsage::RsrcOrigin rsrc_origin_ = IRsrcUsage::RsrcOrigin::Unknown

		);

	public:
		void AddSemToWaitOn(std::shared_ptr<TimelineSemWrapper> sync_sem, uint64_t wait_val = 1)
		{
			wait_sync_semas.push_back(std::make_pair(std::move(sync_sem), wait_val));
		}

		void AddSemToSignal(std::shared_ptr<TimelineSemWrapper> sync_sem, uint64_t signal_val = 1)
		{
			signal_sync_semas.push_back(std::make_pair(std::move(sync_sem), signal_val));
		}


		void AddSemToWaitOn(std::shared_ptr<BinarySemWrapper> sync_sem)
		{
			wait_sync_semas_binary.push_back(std::move(sync_sem));
		}

		void AddSemToSignal(std::shared_ptr<BinarySemWrapper> sync_sem)
		{
			signal_sync_semas_binary.push_back(std::move(sync_sem));
		}

	public:
		S source_sync_info;
		S target_sync_info;
		Queue* source_queue;
		Queue* target_queue;
		typename VRsrcHandle<R>::type underlying_vrsrc;
		IRsrcUsage::RsrcOrigin rsrc_origin;

	public:
		std::vector<std::pair<std::shared_ptr<TimelineSemWrapper>, uint64_t>> signal_sync_semas;
		std::vector<std::pair<std::shared_ptr<TimelineSemWrapper>, uint64_t>> wait_sync_semas;

		std::vector<std::shared_ptr<BinarySemWrapper>> wait_sync_semas_binary;
		std::vector<std::shared_ptr<BinarySemWrapper>> signal_sync_semas_binary;
	};


	template <typename R, typename S>
	SyncInfoDiffQueue<R, S>::SyncInfoDiffQueue(
		S source_sync_info_,
		S target_sync_info_,
		Queue* source_queue_,
		Queue* target_queue_,
		typename VRsrcHandle<R>::type underlying_vrsrc_,
		IRsrcUsage::RsrcOrigin rsrc_origin_

	) :
		source_sync_info(source_sync_info_),
		target_sync_info(target_sync_info_),

		source_queue(source_queue_),
		target_queue(target_queue_),

		underlying_vrsrc(underlying_vrsrc_),
		rsrc_origin(rsrc_origin_)
	{
	}


	template <typename R, typename S>
	SyncInfoSameQueue<R, S>::SyncInfoSameQueue(
		S source_sync_info_,
		S target_sync_info_,
		typename VRsrcHandle<R>::type underlying_vrsrc_,
		IRsrcUsage::RsrcOrigin rsrc_origin_

	)
		:
		source_sync_info(source_sync_info_),
		target_sync_info(target_sync_info_),
		underlying_vrsrc(underlying_vrsrc_),
		rsrc_origin(rsrc_origin_)

	{
	}
}
