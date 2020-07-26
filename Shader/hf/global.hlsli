#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "shaderInterop.h"
#include "shaderInteropRender.h"

#define TEXTURE_BASECOLOR_MAP	TEXTURE_SLOT_0
#define TEXTURE_NORMAL_MAP		TEXTURE_SLOT_1
#define TEXTURE_SURFACE_MAP		TEXTURE_SLOT_2

// common texture slots
TEXTURE2DARRAY(texture_shadowmap_array, float, TEXTURE_SLOT_SHADOW_ARRAY_2D);
TEXTRRECUBEARRAY(texture_shadowcubemap_array, float, TEXTURE_SLOT_SHADOW_ARRAY_CUBE);

//TEXTURE_SLOT_SHADOW_ARRAY_CUBE
TEXTRRECUBE(texture_global_env_map, float4, TEXTURE_SLOT_GLOBAL_ENV_MAP);

TEXTURE2D(texture_0, TEXTURE_SLOT_0);
TEXTURE2D(texture_1, TEXTURE_SLOT_1);
TEXTURE2D(texture_2, TEXTURE_SLOT_2);
TYPE_TEXTURE2D(texture_depth, float, TEXTURE_SLOT_DEPTH);
TYPE_TEXTURE2D(texture_linear_depth, float, TEXTURE_SLOT_LINEAR_DEPTH);
 
// common sampler states
SAMPLERSTATE(sampler_point_clamp, SAMPLER_POINT_CLAMP_SLOT);
SAMPLERSTATE(sampler_linear_clamp, SAMPLER_LINEAR_CLAMP_SLOT);
SAMPLERSTATE(sampler_anisotropic, SAMPLER_ANISOTROPIC_SLOT);
SAMPLERCOMPARISONSTATE(sampler_comparison_depth, SAMPLER_COMPARISON_SLOT);

// common structred buffer
STRUCTUREDBUFFER(LightArray, ShaderLight, SBSLOT_SHADER_LIGHT_ARRAY);
STRUCTUREDBUFFER(MatrixArray, float4x4, SBSLOT_MATRIX_ARRAY);
STRUCTUREDBUFFER(TiledLights, uint, SBSLOT_TILED_LIGHTS);

// alpha test
#ifdef _ENABLE_ALPHATEST_
#define ALPHATEST(x)	clip((x) - gCommonAlphaCutRef);
#else
#define ALPHATEST(x)
#endif

static const float PI = 3.14159265358979323846;

inline float2 GetScreenSize()
{
    return gFrameScreenSize;
}

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

inline bool IsSaturated(float2 v)
{
    return IsSaturated(v.x) && IsSaturated(v.y);
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

inline uint Flatten(uint2 input, uint2 params)
{
    return input.y * params.x + input.x;
}

inline uint2 UnFlatten(uint input, uint params)
{
    return uint2(
        input % params,
        input / params
    );
}

inline uint2 UnFlatten(uint input, uint2 params)
{
    return uint2(
        input % params.y,
        input / params.y
    );
}

struct Plane
{
    float3 normal;
    float distance;
};

inline Plane ComputePlane(float3 pos1, float3 pos2, float3 pos3)
{
    Plane plane;
    
    float3 v1 = pos2 - pos1;
    float3 v2 = pos3 - pos1;

    plane.normal = normalize(cross(v1, v2));
    plane.distance = dot(plane.normal, pos1); // use dot to compute distance to origin
    return plane;
}

struct AABB
{
    float3 center;
    float3 extents;
    
    float3 GetMin() { return center - extents; }
    float3 GetMax() { return center + extents; }
};

void CreateAABBFromMinMax(inout AABB aabb, float3 minV, float3 maxV)
{
    aabb.center  = (minV + maxV) * 0.5f;
    aabb.extents = (maxV - aabb.center);
}

inline float GetLinearDepth(float depth, float nearZ, float farZ)
{ 
    // NDC z -> view z
    return farZ * nearZ / (nearZ - depth * (nearZ - farZ));
}

inline float GetLinearDepth(float depth)
{
    return GetLinearDepth(depth, gCameraNearZ, gCameraFarZ);
}

inline float GetLinearDepth01(float depth)
{
    return GetLinearDepth(depth) * gCameraInvFarZ; // => [0, 1]
}

// Hammersley采样,均匀分布的2D随机采样
float RadicalInverse(uint bits)
{
    //reverse bit
    //高低16位换位置
    bits = (bits << 16u) | (bits >> 16u);
    //A是5的按位取反
    bits = ((bits & 0x55555555) << 1u) | ((bits & 0xAAAAAAAA) >> 1u);
    //C是3的按位取反
    bits = ((bits & 0x33333333) << 2u) | ((bits & 0xCCCCCCCC) >> 2u);
    bits = ((bits & 0x0F0F0F0F) << 4u) | ((bits & 0xF0F0F0F0) >> 4u);
    bits = ((bits & 0x00FF00FF) << 8u) | ((bits & 0xFF00FF00) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse(i));
}

// Hanmersley球面均匀采样
// https://zhuanlan.zhihu.com/p/103715075
float3 HemisphereSampleUniform(float u, float v)
{
    float phi = v * 2.0 * PI;
    float cosTheta = 1.0 - u;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

// return random float in range [0, 1]
float Rand(inout float seed, in float2 uv)
{
    float result = frac(sin(seed / 100.0f * dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
    seed += 1.0f;
    return result;
}

// 将屏幕坐标转换为相机裁剪空间xy<(-1, 1)
// 再从裁剪空间转换到相机空间
float3 ClipToCamear(float4 clip)
{
    float4 view = mul(gCameraInvP, clip);
    return view.xyz / view.w;
}

float3 ScreenToCamera(float2 uv, float depth)
{
    float4 clip = float4(float2(uv.x, 1.0f - uv.y) * 2.0f - 1.0f, depth, 1.0f);
    return ClipToCamear(clip);
}

inline float4 ConvertColorUintToFloat4(in uint value)
{
    float4 retColor;
    retColor.r = (float) (value & 0x000000ff) / 255.0f;
    retColor.g = (float) ((value >> 8) & 0x000000ff) / 255.0f;
    retColor.b = (float) ((value >> 16) & 0x000000ff) / 255.0f;
    retColor.a = (float) ((value >> 24) & 0x000000ff) / 255.0f;
    return retColor;
}

#endif