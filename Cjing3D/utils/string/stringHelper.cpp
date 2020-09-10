#include "stringHelper.h"

#ifdef _WIN32
#include <windows.h>
#else
#include "utils\string\stringConverth"
#endif // _WIN32

namespace Cjing3D
{
namespace StringHelper
{
	std::string WStringToString(const std::wstring& wstr)
	{
#ifdef _WIN32
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
#else
		return ucs2ToUtf8(wstr);
#endif
	}

	std::wstring StringToWString(const std::string& str)
	{
#ifdef _WIN32
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
#else
		return utf8ToUcs2(str);
#endif
	}
}
}