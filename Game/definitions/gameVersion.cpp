#include "gameVersion.h"

#include <string>

namespace GameVersion
{
	// major version
	const int MajorVersion = 0;
	// features [0-100]
	const int MinorVersion = 0;
	// small update; bug fixes [0-1000]
	const int PatchVersion = 2;
	// version string
	const std::string VersionString = std::to_string(MinorVersion) + "." + std::to_string(MinorVersion) + "." + std::to_string(PatchVersion);

	int GetVersion()
	{
		return MajorVersion * 100000 + MinorVersion * 1000 + PatchVersion;
	}

	const char* GetVersionString()
	{
		return VersionString.c_str();
	}
}


