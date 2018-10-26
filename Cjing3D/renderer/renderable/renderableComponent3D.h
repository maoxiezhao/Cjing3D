#pragma once

#include "renderer\renderable\renderableComponent.h"

namespace Cjing3D
{
	class RenderableComponent3D : public RenderableComponent
	{
	public:
		RenderableComponent3D();
		virtual ~RenderableComponent3D();

		virtual void Initialize();
		virtual void Start();
		virtual void Render();

	protected:
		virtual void RenderScene(GraphicsThread threadID);
	};

	using RenderableComponent3DPtr = std::shared_ptr<RenderableComponent3D>;

}