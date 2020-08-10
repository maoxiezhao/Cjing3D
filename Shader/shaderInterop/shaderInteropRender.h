#ifndef _SHADERINTEROP_RENDER_H_
#define _SHADERINTEROP_RENDER_H_

#include "shaderInterop.h"

///////////////////////////////////////////////////////////////////////////////////////////
// common
CBUFFER(CommonCB, CBSLOT_RENDERER_COMMON)
{
	float  gCommonAlphaCutRef;
	float3 gCommonPadding;
};

CBUFFER(FrameCB, CBSLOT_RENDERER_FRAME)
{
	float2 gFrameScreenSize;
	float2 gFrameInvScreenSize;
	float  gFrameTime;
	float  gFrameDeltaTime;
	float2 gFramePadding;
	// light
	uint   gShaderLightArrayCount;
	float3 gFrameAmbient;
	float  gFrameGamma;
	uint   gFrameShadowCascadeCount;
	uint2  gFrameTileCullingCount;
};

CBUFFER(CameraCB, CBSLOT_RENDERER_CAMERA)
{
	float4x4 gCameraVP;
	float4x4 gCameraView;
	float4x4 gCameraProj;
	float4x4 gCameraInvP;
	float4x4 gCameraInvVP;

	float3   gCameraPos;
	float    gCameraPadding;

	float    gCameraNearZ;
	float    gCameraFarZ;
	float    gCameraInvNearZ;
	float    gCameraInvFarZ;
};

struct ShaderMaterial
{
	float4 baseColor;
	int haveBaseColorMap;
	int haveNormalMap;
	int haveSurfaceMap;
	int useVertexColor;
};

CBUFFER(MaterialCB, CBSLOT_RENDERER_MATERIAL)
{
	ShaderMaterial gMaterial;
};

struct CubemapRenderCamera
{
	float4x4 cubemapVP;
	uint4 properties;
};

CBUFFER(CubemapRenderCB, CBSLOT_CUBEMAP)
{
	CubemapRenderCamera gCubemapRenderCams[6];
};

CBUFFER(RenderMiscCB, CBSLOT_MISC)
{
	float4x4 gMiscTransform;
	float4   gMiscColor;
};

///////////////////////////////////////////////////////////////////////////////////////////
// light
// shader light definitions
static const uint SHADER_MATRIX_COUNT = 128;
static const uint SHADER_LIGHT_COUNT = 128;
// 每个tile用一个U32的位来表示受影响的光源
static const uint SHADER_LIGHT_TILE_BUCKET_COUNT = SHADER_LIGHT_COUNT / 32;

static const uint SHADER_LIGHT_TYPE_DIRECTIONAL = 0;
static const uint SHADER_LIGHT_TYPE_POINT = 1;
static const uint SHADER_LIGHT_TYPE_SPOT = 2;

// tile definitions
static const uint LIGHT_CULLING_TILED_BLOCK_SIZE = 16;
static const uint LIGHT_CULLING_THREAD_SIZE = 8;
static const uint LIGHT_CULLING_GRANULARITY = LIGHT_CULLING_TILED_BLOCK_SIZE / LIGHT_CULLING_THREAD_SIZE;

CBUFFER(CSParamsCB, CBSLOT_CS_PARAMS)
{
	uint2 gCSNumThreads;
	uint2 gCSNumThreadGroups;
	uint  gCSNumLights;
	uint3 gCSNumValues;
};

// light type
struct ShaderLight
{
	float3 viewPosition;
	uint type;
	float3 direction;
	float range;
	float3 worldPosition;
	float energy;
	float4 color;
	float4 extraData;
	float shadowBias;
	uint shadowData;	// 前16位表示光照矩阵索引，后16位表示阴影贴图索引
	float shadowKernel;
	
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
		return shadowData != (~0);
	}

	inline void SetShadowData(uint matrixIndex, uint shadowMapIndex)
	{
		shadowData = matrixIndex & 0xffff;
		shadowData |= (shadowMapIndex & 0xffff) << 16;
	}

	inline uint GetShadowMatrixIndex()
	{
		return shadowData & 0xffff;
	}

	inline uint GetShadowMapIndex()
	{
		return (shadowData >> 16) & 0xffff;
	}

	// cubemap depth 
	inline float GetCubemapDepthK()
	{
		return extraData.x;
	}
	inline float GetCubemapDepthB()
	{
		return extraData.y;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////
// postprocess

static const uint SHADER_POSTPROCESS_BLOCKSIZE = 8;
static const uint SHADER_LINEARDEPTH_BLOCKSIZE = 16;
static const uint SHADER_GAUSSIAN_BLUR_BLOCKSIZE = 256;

CBUFFER(PostprocessCB, CBSLOT_POSTPROCESS)
{
	uint2 gPPResolution;
	float2 gPPInverseResolution;
	float gPPParam1;
	float gPPParam2;
	float gPPParam3;
	float gPPParam4;
};

///////////////////////////////////////////////////////////////////////////////////////////
// mipmap generate

static const uint SHADER_MIPMAPGENERATE_BLOCKSIZE = 8;

CBUFFER(MipmapGenerateCB, CBSLOT_MIPMAPGENERATE)
{
	uint2 gMipmapGenResolution;
	float2 gMipmapInverseResolution;
};

///////////////////////////////////////////////////////////////////////////////////////////
// terrain
CBUFFER(TerrainCB, CBSLOT_TERRAIN)
{
	float4x4 gTerrainTransform;
	uint2 gTerrainResolution;
	float2 gTerrainInverseResolution;
	uint gTerrainHaveWeightDetailMap;
	float gTerrainElevation;
};


///////////////////////////////////////////////////////////////////////////////////////////
// skinning
static const uint SHADER_SKINNING_BLOCKSIZE = 128;


#endif