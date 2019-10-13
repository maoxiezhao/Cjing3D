#include "renderPath.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"

namespace Cjing3D {

	RenderPath::RenderPath(Renderer& renderer) :
		mRenderer(renderer)
	{
	}

	RenderPath::~RenderPath()
	{
	}

	void RenderPath::Update(F32 dt)
	{
		if (mIsBufferInitialized == false) {
			ResizeBuffers();
			mIsBufferInitialized = true;
		}
	}
}