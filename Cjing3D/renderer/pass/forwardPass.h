#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\resource\renderTarget.h"
#include "core\systemContext.hpp"

namespace Cjing3D {

class Renderer;
class World;

class ForwardPass
{
public:
	ForwardPass(SystemContext& context, Renderer& renderer);
	~ForwardPass();

	void Render();
	void Initialize();
	void Uninitialize();

private:
	void SetupFixedState();

	std::unique_ptr<RenderTarget> mRenderTarget;

	SystemContext& mContext;
	Renderer& mRenderer;
	bool mIsInitialized;
};

}