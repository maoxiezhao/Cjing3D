#include "hf\objectHF.hlsli"

PixelInputType main(InputObjectAll input)
{
	PixelInputType Out;

    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

	// world transform
    surface.position = mul(surface.position, worldMat);
    surface.normal = normalize(mul(surface.normal, (float3x3)worldMat));

    Out.pos = mul(gCameraVP, surface.position);
	Out.pos3D = surface.position;
    Out.nor = surface.normal;
    Out.tex = surface.uv;
    Out.color = surface.color;

	return Out;
}