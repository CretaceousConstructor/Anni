#pragma once
#include "QueueManager.h"
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

		IVirtualResource(const IVirtualResource&) = delete;
		IVirtualResource& operator=(const IVirtualResource&) = delete;

		IVirtualResource(IVirtualResource&&) = delete;
		IVirtualResource& operator=(IVirtualResource&&) = delete;

		virtual ~IVirtualResource() = default;


	public:
		std::string                                  name;
		VRsrcType                                    rsrc_type;

		std::map<size_t, std::vector<GraphicsPassNode*>> level_to_passes_attached_to;
		std::map<size_t, std::vector<std::vector<GraphicsPassNode*>>> level_to_passes_attached_to_partitioned;
		//std::map<size_t, std::shared_ptr<TimelineSemWrapper>>   level_to_t_semas;

		std::vector<std::pair<GraphicsPassNode*, Anni::RsrcAccessTypeRG>> passes_access_this_rsrc;

	};

}
