#pragma once

#include "utils\math.h"

namespace Cjing3D
{
	class Frustum
	{
	public:
		Frustum();
		~Frustum();

	private:
		union {
			struct {
				XMVECTOR mleftPlane;
				XMVECTOR mRightPlane;
				XMVECTOR mTopPlane;
				XMVECTOR mBottomPlane;
				XMVECTOR mFarPlane;
				XMVECTOR mNearPlane;
			};
			XMVECTOR mPlanes[6];
		};
	};
}