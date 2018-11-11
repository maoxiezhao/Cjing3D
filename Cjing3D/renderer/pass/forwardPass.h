#pragma once

#include "renderer\renderableCommon.h"
#include "renderer\renderer.h"

namespace Cjing3D {

class ForwardPass
{
public:
	ForwardPass(Renderer& renderer);

	void Render();

private:
	void SetupFixedState();

	Renderer& mRenderer;
};

}