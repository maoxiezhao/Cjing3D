#ifndef _SHADERINTEROP_RENDER_H_
#define _SHADERINTEROP_RENDER_H_

#include "shaderInterop.h"

///////////////////////////////////////////////////////////////////////////////////////////
// common

CBUFFER(FrameCB, CBSLOT_RENDERER_FRAME)
{
	float2 gFrameScreenSize;
	float2 gFramePadding;
	// light
	uint   gShaderLightArrayCount;
	float3 gFrameAmbient;
	float  gFrameGamma;
	uint   gFrameShadowCascadeCount;
};

CBUFFER(CameraCB, CBSLOT_RENDERER_CAMERA)
{
	float4x4 gCameraVP;
	float4x4 gCameraView;
	float4x4 gCameraProj;
	float4x4 gCameraInvVP;

	float3   gCameraPos;
	float    gPadding;
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

CBUFFER(ImageCB, CBSLOT_IMAGE)
{
	float4 gImageCorners[4];
	float4 gImageColor;
};

///////////////////////////////////////////////////////////////////////////////////////////
// light

static const uint SHADER_LIGHT_TYPE_DIRECTIONAL = 0;
static const uint SHADER_LIGHT_TYPE_POINT = 1;
static const uint SHADER_LIGHT_TYPE_SPOT = 2;

struct ShaderLight
{
	float3 viewPosition;
	uint type;
	float3 direction;
	float range;
	float3 worldPosition;
	float energy;
	float4 color;
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
};

///////////////////////////////////////////////////////////////////////////////////////////
// postprocess

static const uint SHADER_POSTPROCESS_BLOCKSIZE = 8;

CBUFFER(PostprocessCB, CBSLOT_POSTPROCESS)
{
	uint2 gPPResolution;
	float2 gPPInverseResolution;
	float gPPParam1;
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