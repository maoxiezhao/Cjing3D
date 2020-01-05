#ifndef _IMAGE_HF_
#define _IMAGE_HF_

#include "global.hlsli"

TEXTURE2D(texture_base, TEXTURE_SLOT_0);

struct ImageVertexToPixel
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

#endif