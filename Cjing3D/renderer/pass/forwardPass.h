#pragma once

#include "renderer\renderableCommon.h"

namespace Cjing3D {

class Renderer;

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