#include "hf\objectHF.hlsli"

PixelInputType main(InputObjectAll input)
{
	PixelInputType Out;

    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

	// world transform
    surface.position = mul(surface.position, worldMat);
    surface.normal = normalize(mul(surface.normal, (float3x3)worldMat));

    Out.pos = mul(surface.position, gCameraVP);
    Out.nor = surface.normal;
    Out.tex = surface.uv;

	return Out;
}