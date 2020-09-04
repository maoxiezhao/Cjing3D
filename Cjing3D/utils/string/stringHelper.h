#pragma once

#include <string>

namespace Cjing3D 
{
namespace StringHelper
{
	std::string  WStringToString(const std::wstring& wstr);
	std::wstring StringToWString(const std::string& str);
}
}