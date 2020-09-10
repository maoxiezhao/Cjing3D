#ifndef _OBJECT_HF_
#define _OBJECT_HF_

#ifdef _TRNAPARENT_
#define _ENABLE_ALPHATEST_
#endif

#include "global.hlsli"
#include "objectInput.hlsli"
#include "brdf.hlsli"
#include "lightingHF.hlsli"

TEXTURE2D(texture_basecolormap, float4, TEXTURE_BASECOLOR_MAP);
TEXTURE2D(texture_normalmap, float4, TEXTURE_NORMAL_MAP);
TEXTURE2D(texture_surfacemap, float4, TEXTURE_SURFACE_MAP);
 
TEXTURE2D(texture_ao, float4, TEXTURE_SLOT_AO);

#ifndef _OBJECT_SAMPLER_
#define _OBJECT_SAMPLER_ sampler_object
#endif

/////////////////////////////////////////////////////////////////////
// Tiled forward lighting
////////////////////////////////////////////////////////////////////
#ifdef _TILED_FORWRAD_LIGHTING_
void TiledForwardLighting(in float2 pixel, in Surface surface, inout Lighting lighting)
{
    const uint2 tilePos = uint2(floor(pixel / LIGHT_CULLING_TILED_BLOCK_SIZE));
    const uint tileIndex = Flatten(tilePos, gFrameTileCullingCount);
    const uint tileBucketAddress = tileIndex * SHADER_LIGHT_TILE_BUCKET_COUNT;
    
    // 对于每个tile获取对应的tileBucket,每个tileBucket为U32整型，每一位1or0表示是否受对应光源影响
    [branch]
    if (gShaderLightArrayCount > 0)
    {
        const uint firstBucket = 0;
        const uint lastBucket = min(gShaderLightArrayCount / 32, max(0, SHADER_LIGHT_TILE_BUCKET_COUNT - 1));
        
        [loop]
        for (uint bucket = firstBucket; bucket <= lastBucket; bucket++)
        {
            uint bucketBits = TiledLights[tileBucketAddress + bucket];
            
            [loop]
            while(bucketBits != 0)
            {
                const uint firstBitlow = firstbitlow(bucketBits);
                const uint lightIndex = firstBitlow + bucket * 32;
                bucketBits ^= 1 << firstBitlow;

                [branch]
                if (lightIndex < gShaderLightArrayCount)
                {
                    ShaderLight light = LightArray[lightIndex];
		
                    switch (light.GetLightType())
                    {
                        case SHADER_LIGHT_TYPE_DIRECTIONAL:
                            DirectionalLight(light, surface, lighting);
                            break;
                        case SHADER_LIGHT_TYPE_POINT:
                            PointLight(light, surface, lighting);
                            break;
                        case SHADER_LIGHT_TYPE_SPOT:
                            break;
                    }
                }
                else
                {
                    bucket = SHADER_LIGHT_TILE_BUCKET_COUNT;
                    break;
                }
            }
        }
        
    }
}
#endif

/////////////////////////////////////////////////////////////////////
// Simple lighting
////////////////////////////////////////////////////////////////////
#ifdef _FORWRAD_LIGHTING_
void ForwardLighting(in Surface surface, inout Lighting lighting)
{
	// 简单光照下，不考虑光源的culling,直接遍历传递过来的所有光源并执行对应光照计算
	for (uint lightIndex = 0; lightIndex < gShaderLightArrayCount; lightIndex++)
	{
		ShaderLight light = LightArray[lightIndex];
		
		switch (light.GetLightType())
		{
		case SHADER_LIGHT_TYPE_DIRECTIONAL:
			DirectionalLight(light, surface, lighting);
			break;
		case SHADER_LIGHT_TYPE_POINT:
			PointLight(light, surface, lighting);
			break;
		case SHADER_LIGHT_TYPE_SPOT:
			break;
		}
	}
}
#endif

float3 ApplyLight(in Surface surface, in Lighting light)
{
    float3 diffuse = light.directDiffuse + light.ambient * surface.occlusion;
    float3 specular = light.directSpecular;
    return surface.albedo * diffuse + specular;
}

#if !defined(_DISABLE_OBJECT_PS_)

/////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color;

    [branch]
    if (gMaterial.haveBaseColorMap > 0) {
        color = texture_basecolormap.Sample(_OBJECT_SAMPLER_, input.tex);
        color.rgb = DeGammaCorrect(color.rgb);
    } else {
		color = float4(1.0, 1.0, 1.0, 1.0);
	}
	color *= input.color;
    
    ALPHATEST(color.a);

    // 除以w(viewZ) => NCD
    input.pos2D.xy /= input.pos2D.w;
    
	float3 pos3D = input.pos3D.xyz;
    float2 screenPos = input.pos2D.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f); // NDC => Screen
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
        float3 nor = texture_normalmap.Sample(_OBJECT_SAMPLER_, input.tex).rgb;
        nor = nor.rgb * 2 - 1;
        surface.normal = normalize(mul(nor, TBN));
    }
	
	// 处理反射贴图
    float3 spcularIntensity = float3(1.0f, 1.0f, 1.0f);
    [branch]
    if (gMaterial.haveSurfaceMap > 0) {
        spcularIntensity = texture_surfacemap.Sample(_OBJECT_SAMPLER_, input.tex).rgb;
    }
	
    // SSAO
    float occlusion = 1.0f;
#ifndef _TRNAPARENT_
    occlusion = texture_ao.SampleLevel(sampler_linear_clamp, screenPos, 0.0f).r;
#endif
    
    surface = CreateSurface(
		surface.position,
		surface.normal,
		surface.view,
		color,
	    spcularIntensity,
        occlusion
	);

    Lighting lighting = CreateLighting(0.0f, 0.0f, GetAmbientLight());
    
#ifdef _TILED_FORWRAD_LIGHTING_
    TiledForwardLighting(input.pos.xy, surface, lighting);
#endif
    
#ifdef _FORWRAD_LIGHTING_
    ForwardLighting(surface, lighting);
#endif
		
    color.rgb = ApplyLight(surface, lighting);
    
    return color;
}

#endif
#endif