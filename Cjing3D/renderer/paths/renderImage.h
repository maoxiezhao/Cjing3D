#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{

class Renderer;
	
namespace RenderImage
{
	struct ImageParams
	{
		BlendType mBlendType = BlendType_Opaque;
	};

	void Render(RhiTexture2D & texture, ImageParams params, Renderer& renderer);
}
}