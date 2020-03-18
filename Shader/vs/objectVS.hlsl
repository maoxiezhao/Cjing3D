#include "..\hf\objectHF.hlsli"

PixelInputType main(InputObjectAll input)
{
	PixelInputType Out;

    float4x4 worldMat = MakeWorldMatrixFromInstance(input.instance);
    VertexSurface surface = MakeVertexSurfaceFromInput(input);

    // HLSL 矩阵默认为列矩阵， mul 默认列向量右乘矩阵
    // worldMat在C++端已经按列存储，所以mul右乘
    // gCameraVP则是按行为主，在constantBuffer保存，所以左乘
    
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