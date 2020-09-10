#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"
#include "..\hf\brdf.hlsli"
#include "..\hf\lightingHF.hlsli"

#define _DISABLE_OBJECT_PS_
#include "..\hf\objectHF.hlsli"

SAMPLERSTATE(terrainSampler, SAMPLER_LINEAR_CLAMP_SLOT);

TEXTURE2D(heightMap,      float4, TEXTURE_SLOT_0);
TEXTURE2D(weightTexture,  float4, TEXTURE_SLOT_1);
TEXTURE2D(detailTexture1, float4, TEXTURE_SLOT_2);
TEXTURE2D(detailTexture2, float4, TEXTURE_SLOT_3);
TEXTURE2D(detailTexture3, float4, TEXTURE_SLOT_4);

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
    float2 screenPos = input.pos2D.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f); // NDC => Screen
    float3 view = gCameraPos - pos3D;
    float dist = length(view);

    Surface surface;
    surface.position = pos3D;
    surface.normal = CalculateNormal(input.tex);
    surface.view = normalize(view / dist);
	
    float occlusion = 1.0f;
#ifndef _TRNAPARENT_
    occlusion = texture_ao.SampleLevel(sampler_linear_clamp, screenPos, 0.0f).r;
#endif
    
    surface = CreateSurface(
		surface.position,
		surface.normal,
		surface.view,
		color,
	    float3(1.0f, 1.0f, 1.0f),
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