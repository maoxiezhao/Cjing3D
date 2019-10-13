#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{

class Renderer;
	
namespace RenderImage
{
	void Render(RhiTexture2D& texture, Renderer& renderer);
}
}