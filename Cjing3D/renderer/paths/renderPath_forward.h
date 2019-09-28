#pragma once

#include "renderPath.h"

namespace Cjing3D {

class Renderer;
class World;

class RenderPathForward : public RenderPath
{
public:
	RenderPathForward(Renderer& renderer);
	~RenderPathForward();

	virtual void Initialize();
	virtual void Uninitialize();
	virtual void Compose();
	virtual void Render();

private:
	void SetupFixedState();
	void RenderScene();
	void RenderComposition();

	std::unique_ptr<RenderTarget> mRTMain;
	std::unique_ptr<RenderTarget> mRTFinal;
};

}