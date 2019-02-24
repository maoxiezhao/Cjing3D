#ifndef _OBJECT_HF_
#define _OBJECT_HF_

#include "objectInput.hlsli"

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 nor : NORMAL;
};


#endif