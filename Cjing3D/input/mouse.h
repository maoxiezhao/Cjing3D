#pragma once

#include "common\common.h"
#include "keyCode.h"

namespace Cjing3D
{
	class Mouse
	{
	public:
		Mouse() = default;

		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Uninitialize() = 0;

		I32x2 GetMousePos()const { return mMousePos; }
		I32x2 GetMouseDelta()const { return mMouseDelta; }
		F32 GetMouseWheelDelta()const { return mMouseWheelDelta; }

	private:
		I32x2 mMousePos = I32x2(0, 0);
		I32x2 mMouseDelta = I32x2(0, 0);
		F32 mMouseWheelDelta = 0.0f;
	};
}
