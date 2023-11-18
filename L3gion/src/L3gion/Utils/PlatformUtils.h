#pragma once

#include <string>
#include "L3gion/Core/Buffer.h"

namespace L3gion
{
	class FileDialogs
	{
	public:
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};
	class FileSystem
	{
	public:
		static Buffer readFileBinary(const std::filesystem::path& filepath);
	};
}