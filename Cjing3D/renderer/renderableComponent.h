#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D
{
	class RenderableComponent
	{
	public:


		void Render();
	};

	using RenderableComponentPtr = std::shared_ptr<RenderableComponent>;

}