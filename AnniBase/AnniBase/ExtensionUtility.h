#pragma once

#include "AnniVkHeader.h"
#include "WindowsSubsysHeader.h"
#include <string>
#include <vector>

namespace Anni::ExtensionUtility
{
//global�������˼ָ�����������������򣬶���������ĳ��device��GPU��
	std::vector<const char *>        GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled);
	const std::vector<const char *> &GetRequiredExtensionsForAGoodDevice();

};        // namespace Anni::ExtensionUtility
