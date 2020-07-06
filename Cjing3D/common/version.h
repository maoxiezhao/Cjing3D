#pragma once

namespace CjingVersion
{

#define CJING3D_MAJOR_VERSION 0
#define CJING3D_MINOR_VERSION 0
#define CJING3D_PATCH_VERSION 1

#define CJING3D_VERSION_STR   "0.0.1"
#define CJING3D_VERSION_NUM   100001

inline const char* GetVersion()
{
	return CJING3D_VERSION_STR;
}
}
