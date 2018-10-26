#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiDefinition.h"

namespace Cjing3D
{
	class RenderableComponent
	{
	public:
		RenderableComponent();
		virtual ~RenderableComponent();

		virtual void Initialize();
		virtual void Start();
		virtual void Render();
	};

	using RenderableComponentPtr = std::shared_ptr<RenderableComponent>;

}