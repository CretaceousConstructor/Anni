#include "BufferCI.h"

namespace Anni
{

	bool CI::IsSameType(const BufferCreateInfoEnhanced& lhs, const BufferCreateInfoEnhanced& rhs)
	{
		return (lhs.mem_prop == rhs.mem_prop) && (lhs.vk_buffer_CI.usage == rhs.vk_buffer_CI.usage);
	}

}
