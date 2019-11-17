#pragma once

#include "renderPath.h"
#include "gui\gui.h"

namespace Cjing3D {

	class RenderPath2D : public RenderPath
	{
	public:
		RenderPath2D(Renderer& renderer);
		~RenderPath2D();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void Update(F32 dt);
		virtual void Render();
		virtual void Compose();
		virtual void ResizeBuffers();

	private:
		Texture2D mRenderTargetFinal;
		GUIRenderer mGUIRenderer;
	};

}