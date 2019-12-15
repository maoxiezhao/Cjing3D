#ifndef _OBJECT_HF_
#define _OBJECT_HF_

#include "global.hlsli"
#include "objectInput.hlsli"
#include "brdf.hlsli"
#include "lightingHF.hlsli"

struct PixelInputType
{
	float4 pos		: SV_POSITION;
	float4 pos3D	: WORLDPOSITION;
	float2 tex		: TEXCOORD0;
	float3 nor		: NORMAL;
    float4 color	: COLOR;
};

TEXTURE2D(texture_basecolormap, TEXTURE_BASECOLOR_MAP);
TEXTURE2D(texture_normalmap, TEXTURE_NORMAL_MAP);
TEXTURE2D(texture_surfacemap, TEXTURE_SURFACE_MAP);

float4 ForwardLighting(in Surface surface)
{
	return float4(1, 1, 1, 1);
}

float4 SimpleLighting(in Surface surface)
{
	float3 color = GetAmbientLight().rgb;

	// 简单光照下，不考虑光源的culling,直接遍历传递过来的所有光源并执行对应光照计算
	for (uint lightIndex = 0; lightIndex < gShaderLightArrayCount; lightIndex++)
	{
		ShaderLight light = LightArray[lightIndex];
		
		switch (light.GetLightType())
		{
		case SHADER_LIGHT_TYPE_DIRECTIONAL:
			color += DirectionalLight(light, surface);
			break;
		case SHADER_LIGHT_TYPE_POINT:
			color += PointLight(light, surface);
			break;
		case SHADER_LIGHT_TYPE_SPOT:
			break;
		}
	}

	return float4(color.rgb, 1.0f);
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

	float3 pos3D = input.pos3D.xyz;
	float3 view = gCameraPos - pos3D;
	float dist = length(view);

	Surface surface = CreateSurface(
		pos3D,
		normalize(input.nor),
		normalize(view / dist),
		input.color
	);

#ifndef _SIMPLE_BASE_LIGHT_
	color *= ForwardLighting(surface);
#else
	color *= SimpleLighting(surface);
#endif

	return color;
}


#endif