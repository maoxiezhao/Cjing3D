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


#endif