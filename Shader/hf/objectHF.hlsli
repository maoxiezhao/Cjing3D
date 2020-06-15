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
 
/////////////////////////////////////////////////////////////////////
// Tiled forward lighting
////////////////////////////////////////////////////////////////////
#ifdef _TILED_FORWRAD_LIGHTING_
float4 TiledForwardLighting(in float2 pixel, in Surface surface)
{
    const uint2 tilePos = uint2(floor(pixel / LIGHT_CULLING_TILED_BLOCK_SIZE));
    const uint tileIndex = Flatten(tilePos, gFrameTileCullingCount);
    const uint tileBucketAddress = tileIndex * SHADER_LIGHT_TILE_BUCKET_COUNT;
    
    // ����ÿ��tile��ȡ��Ӧ��tileBucket,ÿ��tileBucketΪU32���ͣ�ÿһλ1or0��ʾ�Ƿ��ܶ�Ӧ��ԴӰ��
    float3 color = GetAmbientLight().rgb;
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
                            color += DirectionalLight(light, surface);
                            break;
                        case SHADER_LIGHT_TYPE_POINT:
                            color += PointLight(light, surface);
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

    return float4(color.rgb, 1.0f);
}
#endif

/////////////////////////////////////////////////////////////////////
// Simple lighting
////////////////////////////////////////////////////////////////////
#ifdef _FORWRAD_LIGHTING_
float4 ForwardLighting(in Surface surface)
{
	float3 color = GetAmbientLight().rgb;

	// �򵥹����£������ǹ�Դ��culling,ֱ�ӱ������ݹ��������й�Դ��ִ�ж�Ӧ���ռ���
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
#endif

#if !defined(_DISABLE_OBJECT_PS_)

/////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////

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

	// ������ڷ�����ͼ������ݷ�����ͼ�޸ķ���
    [branch]
    if (gMaterial.haveNormalMap > 0)
	{
		float3x3 TBN = ComputeTangateTransform(surface.normal, surface.position, input.tex);
        float3 nor = texture_normalmap.Sample(sampler_linear_clamp, input.tex).rgb;
        nor = nor.rgb * 2 - 1;
        surface.normal = normalize(mul(nor, TBN));
    }
	
	// ��������ͼ
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

#ifdef _TILED_FORWRAD_LIGHTING_
    color *= TiledForwardLighting(input.pos.xy, surface);
#endif
    
#ifdef _FORWRAD_LIGHTING_
    color *= ForwardLighting(surface);
#endif
		
	return color;
}

#endif
#endif