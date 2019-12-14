#ifndef _LIEGHTING_HF_
#define _LIEGHTING_HF_

inline float3 GetAmbientLight()
{
	return float3(0.5f, 0.5f, 0.5f);
}

#ifdef _SIMPLE_BASE_LIGHT_
// phong Lighting

inline float3 DirectionalLight(in ShaderLight light, in Surface surface)
{
	return float3(0, 0, 0);
}

inline float3 PointLight(in ShaderLight light, in Surface surface)
{
	float3 color = light.color.rgb;
	float3 lightDir = normalize(light.worldPosition - surface.position);
	float diff = max(dot(lightDir, surface.normal), 0.0f);

	return color * diff;
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