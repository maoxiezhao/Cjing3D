#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "shaderInterop.h"
#include "shaderInteropRender.h"

// common texture slots
TEXTURE2D(texture_0, TEXTURE_SLOT_0);
TEXTURE2D(texture_1, TEXTURE_SLOT_1);
TEXTURE2D(texture_2, TEXTURE_SLOT_2);

// common sampler states
SAMPLERSTATE(sampler_linear_clamp, SAMPLER_LINEAR_CLAMP_SLOT);
SAMPLERSTATE(sampler_anisotropic, SAMPLER_ANISOTROPIC_SLOT);

// common structred buffer
STRUCTREDBUFFER(LightArray, ShaderLight, SHADER_LIGHT_ARRAY_SLOT);

// 计算法线切线空间变换矩阵
inline float3x3 ComputeTangateTransform(float3 N, float3 P, float2 UV)
{
	// dP = [TB] * dUV 
	// dUV-1 * dp = [TB]
	float3 dP1 = ddx_coarse(P);
	float3 dP2 = ddy_coarse(P);
	float2 dUV1 = ddx_coarse(UV);
	float2 dUV2 = ddy_coarse(UV);
	
	float3x3 M = float3x3(dP1, dP2, cross(dP1, dP2));
	float2x3 inverseM = float2x3(cross(M[1], M[2]), cross(M[2], M[0]));
	
	float3 T = normalize(mul(float2(dUV1.x, dUV2.x), inverseM));
	float3 B = normalize(mul(float2(dUV1.y, dUV2.y), inverseM));
	return float3x3(T, B, N);
}

// gamma correction
inline float3 GammaCorrect(float3 color)
{
    return pow(color, 1.0f / gFrameGamma);
}
inline float3 DeGammaCorrect(float3 color)
{
    return pow(color, gFrameGamma);
}

// RGB->YUV
inline float RGBToLuminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

#endif