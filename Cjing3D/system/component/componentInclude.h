#pragma once

#include "system\ecsSystem.h"
#include "system\component\component.h"
#include "renderer\renderableCommon.h"
#include "renderer\RHI\rhiDefinition.h"
#include "renderer\RHI\rhiBuffer.h"
#include "renderer\RHI\rhiTexture.h"

namespace Cjing3D
{
	const XMFLOAT4X4 IDENTITYMATRIX = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}