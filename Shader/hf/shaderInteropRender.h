#ifndef _SHADERINTEROP_RENDER_H_
#define _SHADERINTEROP_RENDER_H_

#include "shaderInterop.h"


CBUFFER(FrameCB, CBSLOT_RENDERER_FRAME)
{
	float2 gFrameScreenSize;
	float2 gFramePadding;
	// light
	uint   gShaderLightArrayCount;
	float3 gFrameAmbient;
	float  gFrameGamma;
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
	int haveNormalMap;
	int haveSurfaceMap;
};

CBUFFER(MaterialCB, CBSLOT_RENDERER_MATERIAL)
{
	ShaderMaterial gMaterial;
};

CBUFFER(ImageCB, CBSLOT_IMAGE)
{
	float4 gImageCorners[4];
	float4 gImageColor;
};

///////////////////////////////////////////////////////////////////////////////////////////

struct ShaderLight
{
	float3 viewPosition;
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

static const uint SHADER_LIGHT_TYPE_DIRECTIONAL = 0;
static const uint SHADER_LIGHT_TYPE_POINT = 1;
static const uint SHADER_LIGHT_TYPE_SPOT = 2;

///////////////////////////////////////////////////////////////////////////////////////////

// postprocess
static const uint SHADER_POSTPROCESS_BLOCKSIZE = 8;

CBUFFER(PostprocessCB, CBSLOT_POSTPROCESS)
{
	uint2 gPPResolution;
	float2 gPPInverseResolution;
	float gPPParam1;
};

#endif