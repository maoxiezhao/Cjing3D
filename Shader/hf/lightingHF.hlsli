#ifndef _LIEGHTING_HF_
#define _LIEGHTING_HF_

inline float3 GetAmbientLight()
{
	return gFrameAmbient;
}

#ifdef _SIMPLE_BASE_LIGHT_

// Blinn-Phong Lighting
static const float pointLightAttConstant = 1.0f;
static const float pointLightAttLinear = 0.09f;
static const float pointLightAttQuadratic = 0.032f;

inline float3 DirectionalLight(in ShaderLight light, in Surface surface)
{
	return float3(0, 0, 0);
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
			//float spec = pow(max(dot(lightDir, surface.reflectView), 0.0f), 32);
			float spec = pow(max(dot(lightDir, surface.normal), 0.0f), 32);
			float3 specularColor = lightColor * spec * attenuation;

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