#pragma once

#include "common\common.h"
#include "keyCode.h"

namespace Cjing3D
{
	class Gamepad
	{
	public:
		Gamepad() = default;

		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Uninitialize() = 0;
	};
}
