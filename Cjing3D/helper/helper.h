#pragma once

#include "common\common.h"

namespace Cjing3D
{
	// 通用的helper函数
	namespace Helper
	{
		void SplitPath(const std::string& fullpath, std::string& path, std::string& fileName)
		{
			size_t found = fullpath.find_last_of("/\\");
			path = fullpath.substr(0, found + 1);
			fileName = fullpath.substr(found + 1);
		}

		std::string ReplaceString(const std::string& srcString, const std::string& subString, const std::string& dstString)
		{
			std::string ret = srcString;
			size_t pos = std::string::npos;
			while ((pos = ret.find(subString)) != std::string::npos) {
				ret.replace(pos, subString.length(), dstString.c_str());
			}
			
			return ret;
		}
	}
}