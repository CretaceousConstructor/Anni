#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "AnniVkHeader.h"
#include <iostream>
namespace Anni::Util
{
	namespace TypeHash
	{
		template <typename T>
		struct TypeIdentifier
		{
			static void id()
			{
			}
		};

		struct TypeIdHash
		{
			std::size_t operator()(void (*id)()) const
			{
				return std::hash<void *>{}(reinterpret_cast<void *>(id));
			}
		};

		struct TypeIdEqual
		{
			bool operator()(void(*id1)(), void(*id2)()) const
			{
				return id1 == id2;
			}
		};
	}



	[[nodiscard]] std::vector<char> ReadFile(const std::string& filename);
	[[nodiscard]] std::string GetFileExtensionName(const std::string& filename);



}        // namespace Anni::Util