#include "AnniUtil.h"

namespace Anni::Util
{
std::string GetFileExtensionName(const std::string &filename)
{
	const size_t pos            = filename.find_last_of('.');
	auto         file_extension = filename.substr(pos + 1, filename.size());

	return file_extension;
}

std::vector<char> ReadFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open shader file!");
	}

	const auto        file_size = file.tellg();
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();

	return buffer;
}




}        // namespace Anni::Util
