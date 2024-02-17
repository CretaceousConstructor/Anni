#pragma once
#include "DeviceManager.h"
namespace Anni
{


	class PipelineLayoutManager
	{
	public:
		PipelineLayoutManager(DeviceManager& device_manager_)
			:device_manager(device_manager_)
		{


		}

		PipelineLayoutManager(const PipelineLayoutManager&) = delete;
		PipelineLayoutManager& operator=(const PipelineLayoutManager&) = delete;

		PipelineLayoutManager(PipelineLayoutManager&& rhs) = delete;
		PipelineLayoutManager& operator=(PipelineLayoutManager&&) = delete;



	private:
		DeviceManager& device_manager;

	};


}
