#pragma once

#include "renderer\renderable\renderableComponent3D.h"

namespace Cjing3D
{
	class ForwardRenderableComponent : public RenderableComponent3D
	{
	public:
		ForwardRenderableComponent();
		virtual ~ForwardRenderableComponent();

		virtual void Initialize();
		virtual void Start();
		virtual void Render();
	};

	using ForwardRenderableComponentPtr = std::shared_ptr<ForwardRenderableComponent>;

}