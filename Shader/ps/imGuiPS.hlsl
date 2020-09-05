#include "..\hf\global.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{
    return input.color * texture_0.Sample(sampler_linear_clamp, input.uv);
}