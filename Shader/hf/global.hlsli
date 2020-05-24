#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "shaderInterop.h"
#include "shaderInteropRender.h"

#define TEXTURE_BASECOLOR_MAP	TEXTURE_SLOT_0
#define TEXTURE_NORMAL_MAP		TEXTURE_SLOT_1
#define TEXTURE_SURFACE_MAP		TEXTURE_SLOT_2

// common texture slots
TEXTURE2DARRAY(texture_shadowmap_array, float, TEXTURE_SLOT_SHADOW_ARRAY_2D);
TEXTRRECUBE(texture_global_env_map, float4, TEXTURE_SLOT_GLOBAL_ENV_MAP);

TEXTURE2D(texture_0, TEXTURE_SLOT_0);
TEXTURE2D(texture_1, TEXTURE_SLOT_1);
TEXTURE2D(texture_2, TEXTURE_SLOT_2);

// common sampler states
SAMPLERSTATE(sampler_linear_clamp, SAMPLER_LINEAR_CLAMP_SLOT);
SAMPLERSTATE(sampler_anisotropic, SAMPLER_ANISOTROPIC_SLOT);
SAMPLERCOMPARISONSTATE(sampler_comparison_depth, SAMPLER_COMPARISON_SLOT);

// common structred buffer
STRUCTUREDBUFFER(LightArray, ShaderLight, SBSLOT_SHADER_LIGHT_ARRAY);
STRUCTUREDBUFFER(MatrixArray, float4x4, SBSLOT_MATRIX_ARRAY);

// alpha test
#ifdef _ENABLE_ALPHATEST_
#define ALPHATEST(x)	clip((x) - gCommonAlphaCutRef);
#else
#define ALPHATEST(x)
#endif

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

// 判断v是否在[0, 1]
inline bool IsSaturated(float v)
{
    return v == saturate(v);
}

inline bool IsSaturated(float3 v)
{
    return IsSaturated(v.x) && IsSaturated(v.y) && IsSaturated(v.z);
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

inline float3 DeGammaCorrectSky(float3 color)
{
    return pow(color, gFrameGamma);
}

// RGB->YUV
inline float RGBToLuminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

inline float3 GetNormalFromNumber(uint norNumber)
{
    float3 nor = 0.0f;
    nor.x = (float) (norNumber & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    nor.y = (float) ((norNumber >> 8) & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    nor.z = (float) ((norNumber >> 16) & 0x000000ff) / 255.0f * 2.0f - 1.0f;
    return nor;
}

inline uint SetNormalIntoNumber(float3 nor)
{
    uint norNumber = 0;
    norNumber |= ((uint) ((nor.x * 0.5f + 0.5f) * 255.0f))  << 0;
    norNumber |= ((uint) ((nor.y * 0.5f + 0.5f) * 255.0f)) << 8;
    norNumber |= ((uint) ((nor.z * 0.5f + 0.5f) * 255.0f)) << 16;
    return norNumber;
}

#endif