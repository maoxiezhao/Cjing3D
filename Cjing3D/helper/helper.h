#pragma once

#include "common\common.h"

namespace Cjing3D
{
	namespace Helper
	{
		void SplitPath(const std::string& fullpath, std::string& path, std::string& fileName)
		{
			size_t found = fullpath.find_last_of("/\\");
			path = fullpath.substr(0, found + 1);
			fileName = fullpath.substr(found + 1);
		}

	}
}