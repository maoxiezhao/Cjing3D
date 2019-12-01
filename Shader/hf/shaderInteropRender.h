#ifndef _SHADERINTEROP_RENDER_H_
#define _SHADERINTEROP_RENDER_H_

#include "shaderInterop.h"

CBUFFER(FrameCB, CBSLOT_RENDERER_FRAME)
{
	float2 gFrameScreenSize;
};

CBUFFER(CameraCB, CBSLOT_RENDERER_CAMERA)
{
	float4x4 gCameraVP;
	float4x4 gCameraView;
	float4x4 gCameraProj;
	float3   gCameraPos;
	float    gPadding;
};

struct ShaderMaterial
{
	float4 baseColor;
	int haveBaseColorMap;
};

CBUFFER(MaterialCB, CBSLOT_RENDERER_MATERIAL)
{
	ShaderMaterial gMaterial;
};

struct ShaderLight
{
	float3 position;
	uint type;
	float3 direction;
	uint range;
	float3 worldPosition;
	uint energy;
	float4 color;

	inline uint GetLightType()
	{
		return type;
	}

	inline void SetLightType(uint t)
	{
		type = t;
	}

	inline bool IsCastingShadow()
	{
		return false;
	}
};

#endif