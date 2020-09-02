#pragma once

#include "renderPath_forward.h"

namespace Cjing3D {

	class RenderPathTiledForward : public RenderPathForward
	{
	public:
		RenderPathTiledForward();
		~RenderPathTiledForward();

		virtual void Render();
	};
}