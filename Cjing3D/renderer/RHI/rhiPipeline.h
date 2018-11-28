#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiResource.h"

namespace Cjing3D
{

class GraphicsDevice;

class RHIPipeline
{
public:
	RHIPipeline(GraphicsDevice& device);
	~RHIPipeline();

	
	void SetViewport(ViewPort viewport);

	void Bind();
	void Clear();

private:
	GraphicsDevice& mDevice;

};
}