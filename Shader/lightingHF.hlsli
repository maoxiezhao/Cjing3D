#ifndef _LIEGHTING_HF_
#define _LIEGHTING_HF_

#include "global.hlsli"
#include "brdf.hlsli"

inline float4 DirectionalLight(in ShaderLight light, in Surface surface)
{
	return float4(0, 0, 0, 0);
}

#endif