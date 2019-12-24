#pragma once

#include "renderPath.h"
#include "gui\guiRenderer.h"

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
		void RenderGUI();

	protected:
		virtual Texture2D* GetDepthBuffer() {
			return nullptr;
		}

	private:
		Texture2D mRTFinal;
	};

}