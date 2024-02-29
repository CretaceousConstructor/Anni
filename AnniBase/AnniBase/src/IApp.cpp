#include "IApp.h"
namespace Anni
{
	IApp::IApp()
	{
#if ( VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1 )
		// initialize minimal set of function pointers
		VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif

	}

	IApp::~IApp() = default;
}

