#ifndef _LIEGHTING_HF_
#define _LIEGHTING_HF_

#include "global.hlsli"
#include "brdf.hlsli"

inline float3 GetAmbientLight()
{
	return gFrameAmbient;
}

#define _SIMPLE_BASE_LIGHT_

#ifdef _SIMPLE_BASE_LIGHT_

// Blinn-Phong Lighting
static const float pointLightAttConstant = 1.0f;
static const float pointLightAttLinear = 0.09f;
static const float pointLightAttQuadratic = 0.032f;

inline float cascadeShadow(float3 shadowPos, float2 shadowTex, float bias, float slice)
{
    // depth [0. -1]
    float currentDepth = shadowPos.z + bias;
    float shadow = 1.0f;
    
    // 如果采样的深度大于当前深度，则返回1，相反则返回0
    shadow = texture_shadowmap_array.SampleCmpLevelZero(sampler_comparison_depth, float3(shadowTex, slice), currentDepth).r;
    
    return shadow;
}

inline float3 DirectionalLight(in ShaderLight light, in Surface surface)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float3 lightDir = normalize(light.direction);

    float NdotL = dot(lightDir, surface.normal);
    if (NdotL > 0) {
        color = light.color.rgb * light.energy * NdotL;
        
        // CSM 阴影
        if (light.IsCastingShadow())
        {
            float shadow = 1.0f;
            uint matrixIndex = light.GetShadowMatrixIndex();
            uint shadowMapIndex = light.GetShadowMapIndex();
            
            // 对于每一个级联，将表面的点通过光源变换矩阵变换到光源空间[-1,1]
            // 同时转换到[0,1]表示为纹理坐标
            for (uint cascade = 0; cascade < gFrameShadowCascadeCount; cascade++)
            {
                float3 shPos = mul(MatrixArray[matrixIndex + cascade], float4(surface.position, 1.0f)).xyz;
                float3 shTex = shPos * float3(0.5f, -0.5f, 0.5f) + 0.5f; // uv坐标中v值和y值方向相反，所以乘以-0.5
                
                if (IsSaturated(shTex))
                {
                    float currentShadow = cascadeShadow(shPos, shTex.xy, light.shadowBias, shadowMapIndex + cascade);

                    // 计算当前等级的阴影值后，判断一下当前位置是否大于0.8，并获取一个factor[0, 1]，
					// 和下一级的shadowvalue做一个线性混合
                    //float3 cascadePosFactor = saturate(saturate(abs(shPos)) - 0.8) * 0.5f;
                    //float cascadeFactor = max(cascadePosFactor.x, max(cascadePosFactor.y, cascadePosFactor.z));
                    //if (cascadeFactor > 0 && cascade < (gFrameShadowCascadeCount - 1))
                    //{
                    //    uint nextCascade = cascade + 1;

                    //    shPos = mul(MatrixArray[matrixIndex + nextCascade], float4(surface.position, 1.0f)).xyz;
                    //    shTex = shPos * float3(0.5f, -0.5f, 0.5f) + 0.5f; 
                    //    float nextShadow = cascadeShadow(shPos, shTex.xy, 0.0f, shadowMapIndex + cascade);
                        
                    //    shadow = lerp(currentShadow, nextShadow, cascadeFactor);
                    //}
                    //else
                    //{
                        shadow = currentShadow;
                    //}
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