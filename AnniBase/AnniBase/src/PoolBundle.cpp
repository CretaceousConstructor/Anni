#include "PoolBundle.h"
namespace Anni
{
	PoolBundle::PoolBundle(DeviceManager& device_manager_) :
		device_manager(device_manager_), timeline_sem_pool(device_manager_)
	{
	}


}        // namespace Anni
