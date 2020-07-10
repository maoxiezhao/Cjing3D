#ifndef _BRDF_HF_
#define _BRDF_HF_

#include "global.hlsli"

struct Surface
{
	float3 position;
	float3 normal;
	float3 view;
	float4 color;
	float3 reflectView;
    float3 spcularIntensity;
    float  occlusion;
};

inline Surface CreateSurface(
	in float3 position,
	in float3 normal,
	in float3 view,
	in float4 color,
    in float3 spcularIntensity,
    in float  occlusion
)
{
	Surface surface;
	surface.position = position;
	surface.normal = normal;
	surface.view = view;
	surface.color = color;
	surface.reflectView = -reflect(view, normal);
    surface.spcularIntensity = spcularIntensity;
    surface.occlusion = occlusion;
    
	return surface;
}


#endif