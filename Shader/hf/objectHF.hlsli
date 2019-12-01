#ifndef _OBJECT_HF_
#define _OBJECT_HF_

#include "global.hlsli"
#include "objectInput.hlsli"
#include "brdf.hlsli"
#include "lightingHF.hlsli"

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 nor : NORMAL;
    float4 color : COLOR;
};

TEXTURE2D(texture_basecolormap, TEXTURE_BASECOLOR_MAP);
TEXTURE2D(texture_normalmap, TEXTURE_NORMAL_MAP);
TEXTURE2D(texture_surfacemap, TEXTURE_SURFACE_MAP);

float4 ForwardLighting(in Surface surface)
{
	return float4(0, 0, 0, 0);
}

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color;

	if (gMaterial.haveBaseColorMap > 0) {
		color = texture_basecolormap.Sample(sampler_linear_clamp, input.tex);
	}
	else
	{
		color = float4(1.0, 1.0, 1.0, 1.0);
	}

	color *= input.color;

	Surface surface = CreateSurface();

#ifdef _SIMPLE_BASE_LIGHT_
	color = ForwardLighting(surface);
#endif

	return color;
}


#endif