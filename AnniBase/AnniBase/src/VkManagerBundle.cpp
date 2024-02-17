#include "VkManagerBundle.h"

VkManagerBundle::VkManagerBundle(GraphicsComponent &gfx) :
    descriptor_manager(gfx),
    pipeline_manager(gfx),
    descriptor_managerv0(gfx)

{}
