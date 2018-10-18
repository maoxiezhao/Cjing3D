#pragma once

#include "renderer\renderableComponent.h"

namespace Cjing3D
{
	class RenderableComponent3D : public RenderableComponent
	{
	public:


		void Render();
	};

	using RenderableComponent3DPtr = std::shared_ptr<RenderableComponent3D>;

}