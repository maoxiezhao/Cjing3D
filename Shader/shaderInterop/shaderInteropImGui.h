#ifndef _SHADERINTEROP_IMGUI_H_
#define _SHADERINTEROP_IMGUI_H_

#include "shaderInterop.h"

CBUFFER(ImGuiCB, CBSLOT_IMGUI)
{
	float4x4 gImGuiWorldTransform;
};

#endif
