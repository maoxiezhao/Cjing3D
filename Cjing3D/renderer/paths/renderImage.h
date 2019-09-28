#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiTexture.h"

namespace Cjing3D
{

class Renderer;
	
namespace RenderImage
{
	void Render(RhiTexture2D& texture, Renderer& renderer);
}
}