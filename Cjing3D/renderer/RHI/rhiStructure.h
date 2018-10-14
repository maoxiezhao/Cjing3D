#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{

	// �ӿڶ���
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