#ifndef _LIEGHTING_HF_
#define _LIEGHTING_HF_

#include "global.hlsli"
#include "brdf.hlsli"

// switch definitions
// ʹ��blin-phong����
#define _SIMPLE_BASE_LIGHT_
// ��������Ӱ
#define ENABLE_SOFT_SHADOW
// ����cascade shadow����
//#define _SHOW_DEBUG_SHADOW_CASCADE 

#ifdef _SHOW_DEBUG_SHADOW_CASCADE
static const float3 debugCascadeColor[3] =
{
    float3(1.0f, 0.0f, 0.0f),
    float3(0.0f, 1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f)
};
#endif

inline float3 GetAmbientLight()
{
	return gFrameAmbient;
}

#ifdef _SIMPLE_BASE_LIGHT_

// Blinn-Phong Lighting
static const float pointLightAttConstant = 1.0f;
static const float pointLightAttLinear = 0.09f;
static const float pointLightAttQuadratic = 0.032f;

static const float softShadowSamplerRange = 1.5f;
static const float defaultMaxShadowBias = 0.002;
inline float cascadeShadow(float3 shadowPos, float2 shadowTex, float bias, float slice, float NdotL, float shadowKernel)
{
    bias = max(defaultMaxShadowBias * (1.0f - NdotL), bias);
    
    // depth [0. -1]
    float currentDepth = shadowPos.z + bias;
    float shadow = 0.0f;
    
#ifdef ENABLE_SOFT_SHADOW
    uint samplerCount = 0;
    for (float x = -softShadowSamplerRange; x <= softShadowSamplerRange; x++)
    {
        for (float y = -softShadowSamplerRange; y <= softShadowSamplerRange; y++)
        {
            float2 currentTex = saturate(shadowTex + float2(x, y) * shadowKernel);
            shadow += texture_shadowmap_array.SampleCmpLevelZero(sampler_comparison_depth, float3(currentTex, slice), currentDepth).r;
            samplerCount++;
        }
    }
    shadow = shadow / (float)samplerCount;
#else
    // �����������ȴ��ڵ�ǰ��ȣ��򷵻�1���෴�򷵻�0
    shadow = texture_shadowmap_array.SampleCmpLevelZero(sampler_comparison_depth, float3(shadowTex, slice), currentDepth).r;
#endif 

    return shadow;
}

inline float3 DirectionalLight(in ShaderLight light, in Surface surface)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float3 lightDir = normalize(light.direction);

    float NdotL = dot(lightDir, surface.normal);
    if (NdotL > 0) {
        color = light.color.rgb * light.energy * NdotL;
        
        // CSM ��Ӱ
        if (light.IsCastingShadow())
        {
            float shadow = 1.0f;
            uint matrixIndex = light.GetShadowMatrixIndex();
            uint shadowMapIndex = light.GetShadowMapIndex();
            
            // ����ÿһ��������������ĵ�ͨ����Դ�任����任����Դ�ռ�[-1,1]
            // ͬʱת����[0,1]��ʾΪ��������
            for (uint cascade = 0; cascade < gFrameShadowCascadeCount; cascade++)
            {
                float3 shPos = mul(MatrixArray[matrixIndex + cascade], float4(surface.position, 1.0f)).xyz;
                float3 shTex = shPos * float3(0.5f, -0.5f, 0.5f) + 0.5f; // uv������vֵ��yֵ�����෴�����Գ���-0.5
                
                if (IsSaturated(shTex))
                {
                    float currentShadow = cascadeShadow(shPos, shTex.xy, light.shadowBias, shadowMapIndex + cascade, NdotL, light.shadowKernel);

                    // ���㵱ǰ�ȼ�����Ӱֵ���ж�һ�µ�ǰλ���Ƿ����0.8������ȡһ��factor[0, 1]��
					// ����һ����shadowvalue��һ�����Ի��
                    float3 cascadePosFactor = saturate(saturate(abs(shPos)) - 0.8f) * 5.0f; //[0, 0.2] => [0, 1]
                    float cascadeFactor = max(cascadePosFactor.x, max(cascadePosFactor.y, cascadePosFactor.z));
                    if (cascadeFactor > 0 && cascade < (gFrameShadowCascadeCount - 1))
                    {
                        uint nextCascade = cascade + 1;

                        shPos = mul(MatrixArray[matrixIndex + nextCascade], float4(surface.position, 1.0f)).xyz;
                        shTex = shPos * float3(0.5f, -0.5f, 0.5f) + 0.5f;
                        float nextShadow = cascadeShadow(shPos, shTex.xy, light.shadowBias, shadowMapIndex + nextCascade, NdotL, light.shadowKernel);
                        
                        shadow = lerp(currentShadow, nextShadow, cascadeFactor);
                    }
                    else
                    {
                        shadow = currentShadow;
                    }
                    
                #ifdef _SHOW_DEBUG_SHADOW_CASCADE
                    color = debugCascadeColor[cascade];
                    return color;
                #endif
                    
                    break;
                }
            }
            
            color *= shadow;
        }
    }
    
    return color;
}

inline float3 PointLight(in ShaderLight light, in Surface surface)
{
	float3 color = float3(0.0f, 0.0f, 0.0f);

	float3 lightV = light.worldPosition - surface.position;
	float dist2 = dot(lightV, lightV);
	float dist = sqrt(dist2);

	if (dist <= light.range)
	{
		float3 lightDir = normalize(lightV / dist);

		float NdotL = dot(lightDir, surface.normal);
		if (NdotL > 0)
		{
			float3 lightColor = light.color.rgb * light.energy;

			float attenuation = 1.0f /
				(pointLightAttConstant + pointLightAttLinear * dist + pointLightAttQuadratic * dist2);

			// diffuse
			float3 diffuseColor = lightColor * NdotL * attenuation;

			// specular
			float3 H = normalize(lightDir + surface.view);
			float spec = pow(max(dot(lightDir, surface.normal), 0.0f), 32);
            float3 specularColor = lightColor * spec * attenuation * surface.spcularIntensity;

			color = diffuseColor + specularColor;
		}
	}

	return color;
}

#else

inline float3 DirectionalLight(in ShaderLight light, in Surface surface)
{
	return float3(0, 0, 0);
}

inline float3 PointLight(in ShaderLight light, in Surface surface)
{
	return float3(0, 0, 0);
}

#endif

#endif