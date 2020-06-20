#pragma once

#include "renderPath3D.h"

namespace Cjing3D {

	class RenderPathTiledForward : public RenderPath3D
	{
	public:
		RenderPathTiledForward();
		~RenderPathTiledForward();

		virtual void ResizeBuffers();
		virtual void Render();

	private:
		Texture2D mRTMain;

		RenderBehavior mRBDepthPrepass;
		RenderBehavior mRBMain;
		RenderBehavior mRBTransparent;
	};
}