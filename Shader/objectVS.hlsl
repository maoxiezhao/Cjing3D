#include "hf\objectHF.hlsli"

PixelInputType main(InputObjectAll input)
{
	PixelInputType Out;

    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

	// world transform
    surface.position = mul(worldMat, surface.position);
    surface.normal = normalize(mul((float3x3) worldMat, surface.normal));

    Out.pos = mul(gCameraVP, surface.position);
    Out.nor = surface.normal;
    Out.tex = surface.uv;
    Out.color = surface.color;

	return Out;
}