#include "..\hf\objectInput.hlsli"
#include "..\hf\global.hlsli"

TEXTURE2D(heightMap, TEXTURE_SLOT_0);

struct HullOutType
{
    float4 pos : POSITIONT;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

struct ConstantOutputType
{
    float EdgeTessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
	
};

[domain("quad")]
PixelInputType main(
    ConstantOutputType input, 
    float2 domain : SV_DomainLocation,
    const OutputPatch<HullOutType, 4> patch)
{
    PixelInputType Output;

    float4 pos = lerp(lerp(patch[0].pos, patch[1].pos, domain.x), lerp(patch[2].pos, patch[3].pos, domain.x), domain.y);
    float2 tex = lerp(lerp(patch[0].tex, patch[1].tex, domain.x), lerp(patch[2].tex, patch[3].tex, domain.x), domain.y);
    float4 color = lerp(lerp(patch[0].color, patch[1].color, domain.x), lerp(patch[2].color, patch[3].color, domain.x), domain.y);

    float elevation = heightMap.SampleLevel(sampler_linear_clamp, tex, 0).r * gTerrainElevation;
    pos.y = elevation;
    
    Output.pos3D = pos;
    Output.pos = mul(gCameraVP, pos);
    Output.tex = tex;
    Output.color = color;
    Output.nor = float3(0.0f, 1.0f, 0.0f);
    
	return Output;
}
