#pragma once

#include "renderPath3D.h"

namespace Cjing3D {

class RenderPathForward : public RenderPath3D
{
public:
	RenderPathForward(Renderer& renderer);
	~RenderPathForward();

	virtual void ResizeBuffers();
	virtual void Render();
	virtual void Compose();

private:
	Texture2D mRTMain;
	Texture2D mRTFinal;
};

}