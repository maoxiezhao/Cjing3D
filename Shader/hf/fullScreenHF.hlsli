#ifndef _FULL_SCREEN_H_
#define _FULL_SCREEN_H_

#include "global.hlsli"

#define screenTexture texture_0

struct FullScreenPixelInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

inline void FullScreenTriangle(in uint vertexID, out float4 pos)
{
    pos.x = (float4) (vertexID / 2) * 2.0f - 1.0f;
    pos.y = (float4) (vertexID % 2) * 2.0f - 1.0f;
    pos.z = 0.0f;
    pos.w = 1.0f;
}

inline void FullScreenTriangleTex(in uint vertexID, out float2 tex)
{
    tex.x = (float) (vertexID / 2) * 2.0f;
    tex.y = 1.0f - (float) (vertexID % 2) * 2.0f;
}


#endif