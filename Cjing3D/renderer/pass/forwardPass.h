#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderTarget.h"
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
	void Compose();

private:
	void SetupFixedState();
	void RenderScene();
	void RenderComposition();

	std::unique_ptr<RenderTarget> mRTMain;
	std::unique_ptr<RenderTarget> mRTFinal;

	SystemContext& mContext;
	Renderer& mRenderer;
	bool mIsInitialized;
};

}