#pragma once

#include "common\common.h"
#include "renderer\RHI\rhiDefinition.h"

namespace Cjing3D
{
	struct PresentConfig
	{
		I32x2 mScreenSize = I32x2(0, 0);
		bool mIsLockFrameRate = false;
		U32 mTargetFrameRate = 60;
		U32 mMultiSampleCount = 1;
		bool mIsFullScreen = false;
		FORMAT mBackBufferFormat = FORMAT_R8G8B8A8_UNORM;
	};
}