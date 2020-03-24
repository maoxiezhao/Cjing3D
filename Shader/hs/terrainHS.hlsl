#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"

// 输入控制点
struct HullInputType
{
    float4 pos   : POSITIONT;
    float2 tex   : TEXCOORD0;
    float4 color : COLOR;
};

// 输出控制点
struct HullOutType
{
    float4 pos : POSITIONT;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

struct ConstantOutputType
{
	float EdgeTessFactor[4]			: SV_TessFactor;
	float InsideTessFactor[2]		: SV_InsideTessFactor; 
};

ConstantOutputType CalcHSPatchConstants(
	InputPatch<HullInputType, 4> patch,
	uint PatchID : SV_PrimitiveID)
{
    ConstantOutputType Output;

    Output.EdgeTessFactor[0] = 1;
    Output.EdgeTessFactor[1] = 1;
    Output.EdgeTessFactor[2] = 1;
    Output.EdgeTessFactor[3] = 1;
    
    Output.InsideTessFactor[0] = 1;
    Output.InsideTessFactor[1] = 1;
    
	return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HullOutType main(InputPatch<HullInputType, 4> patch, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
    HullOutType Output;

    Output.pos   = patch[i].pos;
    Output.tex   = patch[i].tex;
    Output.color = patch[i].color;
    
	return Output;
}
