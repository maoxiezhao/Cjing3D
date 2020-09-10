#ifndef _IMAGE_HF_
#define _IMAGE_HF_

#include "global.hlsli"
#include "..\shaderInterop\shaderInteropImage.h"

TEXTURE2D(texture_base, float4, TEXTURE_SLOT_0);

struct ImageVertexToPixel
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

struct SpritePixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

#endif