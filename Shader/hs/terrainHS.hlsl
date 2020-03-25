#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"

// 输入控制点
struct HullInputType
{
    float4 pos : POSITIONT;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
    uint4  edgeLevel : EDGELEVEL;
    uint2  terrainProp : TERRAINPROP;
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

    uint currentLodLevel = patch[0].terrainProp.x;
    uint4 edgeLodLevel = uint4(currentLodLevel, currentLodLevel, currentLodLevel, currentLodLevel);
    
    [unroll]
    for (uint id = 0; id < 4; id++)
    {
        edgeLodLevel.x = max(patch[id].edgeLevel.x, edgeLodLevel.x);
        edgeLodLevel.y = max(patch[id].edgeLevel.y, edgeLodLevel.y);
        edgeLodLevel.z = max(patch[id].edgeLevel.z, edgeLodLevel.z);
        edgeLodLevel.w = max(patch[id].edgeLevel.w, edgeLodLevel.w);
    }
    
    Output.EdgeTessFactor[0] = max(1, pow(2, max(0, (int) edgeLodLevel.x - (int)currentLodLevel)));
    Output.EdgeTessFactor[1] = max(1, pow(2, max(0, (int) edgeLodLevel.w - (int) currentLodLevel)));
    Output.EdgeTessFactor[2] = max(1, pow(2, max(0, (int) edgeLodLevel.z - (int) currentLodLevel)));
    Output.EdgeTessFactor[3] = max(1, pow(2, max(0, (int) edgeLodLevel.y - (int) currentLodLevel)));
    
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
