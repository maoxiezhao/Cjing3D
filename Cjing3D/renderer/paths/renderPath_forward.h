#pragma once

#include "renderPath3D.h"

namespace Cjing3D {

class RenderPathForward : public RenderPath3D
{
public:
	RenderPathForward();
	~RenderPathForward();

	virtual void ResizeBuffers();
	virtual void Render();

private:
	Texture2D mRTMain;

	RenderBehavior mRBDepthPrepass;
	RenderBehavior mRBMain;
	RenderBehavior mRBTransparent;
};

}