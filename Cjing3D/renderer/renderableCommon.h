#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"
#include "utils\geometry.h"
#include "utils\intersectable.h"
#include "utils\color.h"
#include "helper\stringID.h"

#include <memory>
#include <vector>
#include <string>
#include <list>

#include "hf\shaderInterop.h"
#include "hf\shaderInteropRender.h"
#include "hf\shaderInteropImage.h"

enum BlendType
{
	BlendType_Opaque,
	BlendType_Alpha,
	BlendType_PreMultiplied,
	BlendType_Count,
};

enum RenderingDeviceType
{
	RenderingDeviceType_D3D11,
	RenderingDeviceType_D3D12,
	RenderingDeviceType_Vulkan
};

enum RenderPassType
{
	RenderPassType_Forward,
	RenderPassType_TiledForward,
	RenderPassType_Deferred,
	RenderPassType_Shadow,
	RenderPassType_ShadowCube,
	RenderPassType_Depth,
};

enum RenderableType
{
	RenderableType_Opaque,
	RenderableType_Transparent
};

enum BoundVexterBufferType
{
	BoundVexterBufferType_Nothing,
	BoundVexterBufferType_All,
	BoundVexterBufferType_Pos,
	BoundVexterBufferType_Pos_Tex,
};