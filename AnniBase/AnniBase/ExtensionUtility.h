#pragma once

#include "AnniVkHeader.h"
#include "WindowsSubsysHeader.h"
#include <string>
#include <vector>

namespace Anni::ExtensionUtility
{
//global这里的意思指的是适用于整个程序，而不仅仅是某个device（GPU）
	std::vector<const char *>        GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled);
	const std::vector<const char *> &GetRequiredExtensionsForAGoodDevice();

};        // namespace Anni::ExtensionUtility
