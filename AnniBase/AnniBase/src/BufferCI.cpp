#include "BufferCI.h"

namespace Anni
{

	bool CI::IsSameType(const BufferCreateInfoEnhanced& lhs, const BufferCreateInfoEnhanced& rhs)
	{
		return
			(
				lhs.vk_buffer_CI.usage == rhs.vk_buffer_CI.usage &&
				lhs.vma_allocation_CI.usage == rhs.vma_allocation_CI.usage &&
				lhs.vma_allocation_CI.flags == rhs.vma_allocation_CI.flags
			);
	}

}
