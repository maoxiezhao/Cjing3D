#include "renderPath.h"
#include "renderer\renderer.h"
#include "renderer\RHI\device.h"

namespace Cjing3D {

	RenderPath::RenderPath(Renderer& renderer) :
		mRenderer(renderer),
		mIsInitialized(false)
	{
	}

	RenderPath::~RenderPath()
	{
	}
}