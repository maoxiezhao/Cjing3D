#ifndef _PARTICLE_HF_
#define _PARTICLE_HF_

#include "global.hlsli"
#include "..\shaderInterop\shaderInteropParticle.h"

struct VertexToPixel
{
    float4 pos : SV_Position;
    float4 pos2D : SCREENPOS;
    float4 pos3D : WORLDPOS;
    float2 tex : TEXCOORD0;
    
    nointerpolation uint color : COLOR;
};

#endif