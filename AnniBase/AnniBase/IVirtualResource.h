#pragma once
#include "QueueManager.h"
#include "Resource.h"
#include <map>

namespace Anni::RenderGraphV1
{
	class GraphicsPassNode;
}


namespace Anni::RenderGraphV1
{

	enum class VRsrcType
	{
		Imported,
		Established
	};

	class IVirtualResource
	{
	public:
		IVirtualResource(std::string name_, VRsrcType rsrc_type_);
		IVirtualResource() = delete;

		IVirtualResource(const IVirtualResource&) = default;
		IVirtualResource& operator=(const IVirtualResource&) = default;

		IVirtualResource(IVirtualResource&&) = default;
		IVirtualResource& operator=(IVirtualResource&&) = default;

		virtual ~IVirtualResource() = default;


	public:
		std::string                                  name;
		VRsrcType                                    rsrc_type;

		std::map<size_t, std::vector<GraphicsPassNode*>>                  level_to_passes_attached_to;
		std::map<size_t, std::vector<std::vector<GraphicsPassNode*>>>     level_to_passes_attached_to_partitioned;
		std::vector<std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>> passes_access_this_rsrc;

	};

}
