#pragma once

#include "renderer\renderableCommon.h"
#include "core\systemContext.hpp"

namespace Cjing3D {

class Renderer;
class World;

class ForwardPass
{
public:
	ForwardPass(SystemContext& context);

	void Render(World& world, XMMATRIX transform);

private:
	void SetupFixedState();

	SystemContext& mContext;
};

}