#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"
#include "..\hf\brdf.hlsli"
#include "..\hf\lightingHF.hlsli"

SAMPLERSTATE(terrainSampler, SAMPLER_LINEAR_CLAMP_SLOT);

TEXTURE2D(heightMap,      TEXTURE_SLOT_0);
TEXTURE2D(weightTexture,  TEXTURE_SLOT_1);
TEXTURE2D(detailTexture1, TEXTURE_SLOT_2);
TEXTURE2D(detailTexture2, TEXTURE_SLOT_3);
TEXTURE2D(detailTexture3, TEXTURE_SLOT_4);

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

float3 CalculateNormal(float2 tex)
{
    float stepX = gTerrainInverseResolution.x;
    float stepY = gTerrainInverseResolution.y;
    float hl = heightMap.SampleLevel(terrainSampler, tex + float2(-stepX, 0), 0).r * gTerrainElevation;
    float hr = heightMap.SampleLevel(terrainSampler, tex + float2(stepX, 0), 0).r * gTerrainElevation;
    float ht = heightMap.SampleLevel(terrainSampler, tex + float2(0, stepY), 0).r * gTerrainElevation;
    float hb = heightMap.SampleLevel(terrainSampler, tex + float2(0, -stepY), 0).r * gTerrainElevation;
 
   return normalize(float3(hl - hr, 2.0f, hb - ht));
}

float4 main(PixelInputType input) : SV_TARGET
{
    //float r = heightMap.SampleLevel(sampler_linear_clamp, input.tex, 0).r;
    float4 color = input.color;
    
    // blend texture
    float4 diffColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
   
    [branch]
    if (gTerrainHaveWeightDetailMap > 0)
    {
        float3 weights = weightTexture.SampleLevel(terrainSampler, input.tex, 0).rgb;
        float sumWeight = weights.r + weights.g + weights.b;
        weights /= sumWeight;
        
        float4 color1 = detailTexture1.SampleLevel(terrainSampler, input.tex, 0);
        float4 color2 = detailTexture2.SampleLevel(terrainSampler, input.tex, 0);
        float4 color3 = detailTexture3.SampleLevel(terrainSampler, input.tex, 0);
        
        color1.rgb = DeGammaCorrect(color1.rgb);
        color2.rgb = DeGammaCorrect(color2.rgb);
        color3.rgb = DeGammaCorrect(color3.rgb);
        
        float4 diffColor = (
            weights.r * color1 +
            weights.g * color2 +
            weights.b * color3
        );
    }

    color *= diffColor;
    
    float3 pos3D = input.pos3D.xyz;
    float3 view = gCameraPos - pos3D;
    float dist = length(view);

    Surface surface;
    surface.position = pos3D;
    surface.normal = CalculateNormal(input.tex);
    surface.view = normalize(view / dist);
	
    surface = CreateSurface(
		surface.position,
		surface.normal,
		surface.view,
		input.color,
	    float3(1.0f, 1.0f, 1.0f),
        1.0f
	);

#ifndef _SIMPLE_BASE_LIGHT_
    color *= ForwardLighting(surface);
#else
    color *= SimpleLighting(surface);
#endif
		
    return color;

}