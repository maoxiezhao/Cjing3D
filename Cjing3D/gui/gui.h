#pragma once

#include "common\common.h"

namespace Cjing3D
{
	class GUIRenderer
	{
	public:
		GUIRenderer();
		~GUIRenderer();

		void Update(F32 dt);
		void Render();
	};
}