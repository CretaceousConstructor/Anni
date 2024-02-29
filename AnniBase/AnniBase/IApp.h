#pragma once
#include "AnniVkHeader.h"


namespace Anni
{
	class IApp
	{
	public:
		explicit IApp() ;
	public:
		virtual ~IApp() = 0;

		IApp(const IApp&) = delete;
		IApp& operator=(const IApp&) = delete;

		IApp(IApp&&) = delete;
		IApp& operator=(IApp&&) = delete;
	};
}        // namespace Anni
