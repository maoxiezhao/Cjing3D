#ifndef _OBJECT_INPUT_HF_
#define _OBJECT_INPUT_HF_

#include "shaderInterop.h"
#include "shaderInteropRender.h"

struct InputInstance
{
    float4 wi0 : MAIT0;
    float4 wi1 : MAIT1;
    float4 wi2 : MAIT2;
    float4 color : COLOR;
};

struct InputObjectAll
{
    float4 pos : POSITION_NORMAL_SUBSETINDEX;
    float2 tex : TEXCOORD;

    InputInstance instance;
};

// 表面顶点结构，用于从InputInstance 中转换得到各个成员数据
struct VertexSurface
{
    float4 position;
    float3 normal;
    float4 color;
    uint materialIndex;
    float2 uv;
};

inline float4x4 MakeWorldMatrixFromInstance(InputInstance input)
{
    return float4x4(
		 float4(input.wi0.x, input.wi1.x, input.wi2.x, 0)
		,float4(input.wi0.y, input.wi1.y, input.wi2.y, 0)
		,float4(input.wi0.z, input.wi1.z, input.wi2.z, 0)
		,float4(input.wi0.w, input.wi1.w, input.wi2.w, 1)
	);
}

inline VertexSurface MakeVertexSurfaceFromInput(InputObjectAll input)
{
    VertexSurface surface;
    surface.position = float4(input.pos.xyz, 1.0f);
    surface.color = gMaterial.baseColor;

    uint normalSubsetIndex = input.pos.w;
    surface.normal.x = (float) (normalSubsetIndex & 0x000000ff) / 256.0f * 2.0f - 1.0f;
    surface.normal.y = (float) ((normalSubsetIndex >> 8) & 0x000000ff) / 256.0f * 2.0f - 1.0f;
    surface.normal.z = (float) ((normalSubsetIndex >> 16) & 0x000000ff) / 256.0f * 2.0f - 1.0f;

    surface.materialIndex = (normalSubsetIndex >> 24);

    surface.uv = input.tex;

    return surface;
}

#endif