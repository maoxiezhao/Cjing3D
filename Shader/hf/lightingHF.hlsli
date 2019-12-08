#ifndef _LIEGHTING_HF_
#define _LIEGHTING_HF_

inline float4 GetAmbientLight()
{
	return float4(0.5f, 0.5f, 0.5f, 0.5f);
}

#ifdef _SIMPLE_BASE_LIGHT_
// phong Lighting

inline float4 DirectionalLight(in ShaderLight light, in Surface surface)
{
	return float4(0, 0, 0, 0);
}

inline float4 PointLight(in ShaderLight light, in Surface surface)
{
	return float4(0, 0, 0, 0);
}

#else

inline float4 DirectionalLight(in ShaderLight light, in Surface surface)
{
	return float4(0, 0, 0, 0);
}

inline float4 PointLight(in ShaderLight light, in Surface surface)
{
	return float4(0, 0, 0, 0);
}

#endif

#endif