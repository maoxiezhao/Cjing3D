#pragma once

#include "common\common.h"
#include "helper\IDGenerator.h"
#include "utils\geometry.h"
#include "utils\intersectable.h"

#include <wrl.h>
#include <memory>
#include <vector>
#include <string>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

enum RenderingDeviceType
{
	RenderingDeviceType_D3D11,
	RenderingDeviceType_D3D12,
	RenderingDeviceType_Vulkan
};