#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"
#include "utils\geometry.h"
#include "utils\intersectable.h"
#include "renderer\mappingDefine.h"
#include "helper\stringID.h"

#include <wrl.h>
#include <memory>
#include <vector>
#include <string>

#include "hf\shaderInterop.h"
#include "hf\shaderInteropRender.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

enum RenderingDeviceType
{
	RenderingDeviceType_D3D11,
	RenderingDeviceType_D3D12,
	RenderingDeviceType_Vulkan
};

enum ShaderType
{
	ShaderType_Forward,
	ShaderType_Deferred
};

enum RenderableType
{
	RenderableType_Opaque,
	RenderableType_Transparent
};