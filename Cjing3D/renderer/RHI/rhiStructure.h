#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{

	// 视口定义
	struct ViewPort
	{
		F32 mTopLeftX;
		F32 mTopLeftY;
		F32 mWidth;
		F32 mHeight;
		F32 mMinDepth;
		F32 mMaxDepth;
	};
}