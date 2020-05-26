#ifndef _OBJECT_HF_
#define _OBJECT_HF_

#ifdef _TRNAPARENT_
#define _ENABLE_ALPHATEST_
#endif

#include "global.hlsli"
#include "objectInput.hlsli"
#include "brdf.hlsli"
#include "lightingHF.hlsli"

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

#if !defined(_DISABLE_OBJECT_PS_)

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color;

    [branch]
    if (gMaterial.haveBaseColorMap > 0) {
        color = texture_basecolormap.Sample(sampler_linear_clamp, input.tex);
        color.rgb = DeGammaCorrect(color.rgb);
    } else {
		color = float4(1.0, 1.0, 1.0, 1.0);
	}
	color *= input.color;
    
    ALPHATEST(color.a);

	float3 pos3D = input.pos3D.xyz;
	float3 view = gCameraPos - pos3D;
	float dist = length(view);

	Surface surface;
	surface.position = pos3D;
	surface.normal = normalize(input.nor);
	surface.view = normalize(view / dist);

	// 如果存在法线贴图，则根据法线贴图修改法线
    [branch]
    if (gMaterial.haveNormalMap > 0)
	{
		float3x3 TBN = ComputeTangateTransform(surface.normal, surface.position, input.tex);
        float3 nor = texture_normalmap.Sample(sampler_linear_clamp, input.tex).rgb;
        nor = nor.rgb * 2 - 1;
        surface.normal = normalize(mul(nor, TBN));
    }
	
	// 处理反射贴图
    float3 spcularIntensity = float3(1.0f, 1.0f, 1.0f);
    [branch]
    if (gMaterial.haveSurfaceMap > 0) {
        spcularIntensity = texture_surfacemap.Sample(sampler_linear_clamp, input.tex).rgb;
    }
	
    surface = CreateSurface(
		surface.position,
		surface.normal,
		surface.view,
		input.color,
	    spcularIntensity
	);

#ifndef _SIMPLE_BASE_LIGHT_
    color *= ForwardLighting(surface);
#else
	color *= SimpleLighting(surface);
#endif
		
	return color;
}

#endif
#endif