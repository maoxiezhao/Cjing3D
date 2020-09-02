#pragma once

#include "renderPath.h"
#include "gui\guiRenderer.h"
#include "core\eventSystem.h"

namespace Cjing3D {

	class RenderPath2D : public RenderPath
	{
	public:
		RenderPath2D();
		~RenderPath2D();

		virtual void Initialize();
		virtual void Uninitialize();
		virtual void Update(F32 dt);
		virtual void Render();
		virtual void Compose();
		virtual void ResizeBuffers();

	private:
		virtual void RenderGUI();
		virtual void Render2D();

	protected:
		virtual Texture2D* GetDepthBuffer() {
			return nullptr;
		}

	private:
		Texture2D mRTFinal;
		RenderBehavior mRBFinal;
		ScopedConnection mResolutionChangedConn;
	};

}