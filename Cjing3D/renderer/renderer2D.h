#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\paths\renderPath2D.h"

namespace Cjing3D
{
	class Renderer2D
	{
	public:
		Renderer2D(Renderer& renderer);
		~Renderer2D();

		void Initialize();
		void Uninitialize();
		void Render();
		void SetCurrentRenderPath(RenderPath2D* path) { mRenderPath = path; }
		RenderPath2D* GetCurrentRenderPath() { return mRenderPath; }

	private:
		Renderer & mRenderer;
		RenderPath2D* mRenderPath;
	};

}